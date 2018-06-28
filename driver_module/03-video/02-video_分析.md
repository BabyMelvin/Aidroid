## 分析硬件抽象层
Overlay系统的硬件抽象层是一个硬件模块，本节将介绍Overlay系统的硬件抽象层的基础知识。

### 1.Overlay系统硬件抽象层的接口
文件`hardware/libhardware/include/hardware/overlay.h`定义了Overlay系统硬件抽象层的接口。主要两个结构体:`data device`和`control device`：

<image src="image/02-01.png"/>

* 定义Overlay控制设备和Overlay数据设备：

```c
#define OVERLAY_HARDWARE_CONTROL "control"
#define OVERLAY_HARDWARE_DATA "data"
```

* 定义一个枚举enum，定义了所有支持的Format格式。FrameBuffer会根据Format和width、height来决定Buffer(FrameBuffer里面用来显示的Buffer)大小。

```cpp
enum{
	OVERLYA_FORMAT_RGBA_8888 = HAL_PIXEL_FORMAT_RGBA_8888，
	OVERLYA_FORMAT_RGBA_565 = HAL_PIXEL_FORMAT_RGBA_565，
	OVERLYA_FORMAT_YCbCr_422_SP = HAL_PIXEL_FORMAT_YCbCr_422_SP，
	OVERLYA_FORMAT_YCbCr_420_SP = HAL_PIXEL_FORMAT_YCbCr_420_SP，
	OVERLYA_FORMAT_YCbCr_422_I = HAL_PIXEL_FORMAT_YCbCr_422_I，
	OVERLYA_FORMAT_YCbCr_420_I = HAL_PIXEL_FORMAT_YCbCr_420_I，
	OVERLAY_FORMAT_DEFAULT=99
	//the actual color format is determined by the overlay
};
```

* 定义和Overlay系统相关结构体。
在文件overlay.h和Overlay系统相关的结构体是overlay_t和overlay_handle_t，主要代码如下：

```c
typdef struct overlay_t{
	uint32_t	w;//宽
	uint32_t 	h;//高
	int32_t		format;//颜色格式
	uint32_t	w_stride;//一行内容
	uint32_t	h_stride;//一列内容
	uint32_t	reserved[3];
	/*return a reference to this overlay's handle (the caller doesn't take ownership*/
	overlay_handle_t (*getHanldeRef)(struct overlay_t*overlay);
	uint32_t 	reserved_procs[7];
}overlay_t;
```
结构体overlay_handle_t是内部使用的结构体，用于保存Overlay硬件设备的句柄。在使用的过程中，需要从overlay_t获取overlay_handle_t。其中上一层的使用只实现结构体overlay_handle_t指针的传递，具体操作是在Overlay的硬件层中完成的。

* 定义结构体overlay_control_device_t，此结构体定一个`control device`,里面的成员除了common都是函数，这些函数需要我们去实现。

```cpp
struct overlay_control_device_t{
	struct hw_device_t common;
	int (*get)(struct overlay_control_device_t *dev,int name);
	//建立设备
	overlay_t *(createOverlay)(struct overlay_control_device_t*dev,uint32_t w,uint32_t h,int32_t format);
	//释放资源，分配handle和control device内存
	void(*destroyOverlay)(struct overlay_control_device_t *dev,overlay_t*overlay);
	//设置Overlay显示范围(如果camera的preivew，那么h,w要和preview h,w一致)
	int (*setPostion)(struct overlay_control_device_t *dev,
		overlay_t*verlay,int w,int y,uint32_t w,uint32_t h);
	//获取Overlay显示范围
	int (*getPosition)(struct overlay_control_device_t*dev,
		overlay_t*overlay,int*x,int*y,uint32_t*w,uint32_t*h);
	int (*setParameter)(struct overlay_control_t*dev,overlay_t*overlay);
	int (*stage)(struct overlay_control_device_t*dev,overlay_t*overlay);
	int (*commit)(struct overlay_control_device_t*dev,overlay_t*overlay);
};
```

* 定义结构overlay_data_device_t ，此结构和overlay_control_device_t类似，在具体使用上overlay_control_device_t负责初始化、销毁和控制类的操作，overlay_data_device_t用于显示内存输出的数据操作。

```cpp
struct overlay_data_device_t{
	struct hw_device_t common;
	//通过参数handle来初始化data device
	int(*initalize)(struct overlay_data_device_t*dev,
	overlay_handle_t hadle);
	//重新配置显示参数w,h要想使用两个参数生效，在此需要先关闭，然后再打开
	int(*resizeInput)(struct overlay_data_device_t*dev,
		uint32_t w,uint32_t h);
	//下面分别用于设置显示区域和获取显示区域
	//当播放的时候，需要坐标和宽高来定义如何显示这些数据
	int (*setCrop)(struct overlay_data_device_t*dev,
	uint32_t x,uint32_t y,uint32_t w,uint32_t h);
	int(*getCrop)(struct overlay_data_device_t *dev,
	uint32_t*x,uint32_t*y,uint32_t*w,uint32_t*h);
	int(*setParameter)(struct overlay_data_device_t *dev,
	int param,int value);
	int (*dequeueBuffer)(struct overlay_data_device_t*dev,
		overlay_buffer_t*buf);
	int (*queueBuffer)(struct overlay_data_device_t *dev
		overlay_buffer_t buffer);
	int (*getBufferCount)(struct overlay_data_device_t*dev);
	int (*setFd)(struct overlay_data_device_t*dev,int fd);
};
```

### 2.实现Overlay系统的硬件抽象层
在实现Overlay系统的硬件抽象层时，具体实现方法取决于硬件和驱动程序，根据设备需要进行处理，分为两种情况;

* 1.FrameBuffere驱动程序方式

在此方式下，需要先实现函数`getBufferAddress()`，然后返回通过mmap获得的FrameBuffer指针。如果没有双缓冲的问题，不要要真正实现函数`dequeueBuffer`和`queueBuffer`。上述函数的实现文件overlay.cpp，其路径为:
`hardware/libhardware/module/overlay/overlay.cpp`。

函数getBufferAddress()用于返回FrameBuffer内容显示的内存，通过mmap获取内存地址。

```cpp
void* Overlay::getBufferAddress(overlay_buffer_t buffer)
{
	if(mStatus!=NO_ERROR) return NULL;
	return mOverlay->getBufferAddress(mOverlayData,buffer);
}
```
函数`dequeueBuffer()`和`queueBuffer()`函数:

```cpp
status_t Overlay::dequeueBuffer(overlay_buffer_t*buffer)
{
	if(mStatus!=NO_ERROR) return mStatus;
	return mOverlayData->dequeueBuffer(mOverlayData,buffer);
}
status_t Overlay::queueBuffer(overlay_buffer_t buffer)
{
	if (mStatus!=NO_ERROR) return mStatus;
	return mOverlayData->quueueBuffer(mOverlayData,buffer);
}
```

* 2.Video for Linux2方式
如果使用Linux2输出驱动，函数dequeueBuffer()和queueBuffer()与调用驱动时抓哟ioctl是一致的，即分别调用`VIDIOC_QBUF`和`VIDIOC_DQBUF`可直接实现。其他初始化可以在initalize中进行处理。因为存在视频数据队列，所以此处处理的内容比一般帧缓冲区复杂，但是可以实现更高的性能。

### 3.实现接口
在Android系统中，Overlay系统提供了接口overlay，此接口用于叠加在主显示层上面的另外一个显示层。此叠加的显示层作为视频的输出或相机取景器的预览界面来使用。文件Overlay.h主要内存实现类是Overlay和OverlayRef。OverlayRef需要和surface配合来使用。通过ISurface可以创建出OverlayRef。

```cpp
class Overlay:public virtual RefBase
{
public:
	Overlay(const sp<OverlayRef> overlayRef);
	void destroy();
	//获取Overlay handle,可以更具自己的需要扩展，扩展之后就要很多数据了
	overlay_handle_t getHandleRef() const;
	//获取FrameBuffer用于显示内存地址
	status_t dequeueBuffer(overlay_buffer_t*buffer);
	status_t queueBuffer(overlay_buffer_t*buffer);
	status_t resizeInput(uint32_t width,uint32_t w,uint32_t h);
	status_t getCrop(uint32_t *x,uint32_t*y,uint32_t*w,uint32_t*h);
	status_t setCrop(uint32_t x,uint32_t y,uint32_t w,uint32_t h);
	status_t setParameter(int param,int value);
	void* getBufferAddress(overlay_buffer_t buffer);

	//获取属性信息
	uint32_t getWidth()const;
	uint32_t getHeigth() const;
	int32_t  getFormat() const;
	int32_t getWidthStride() const;
	int32_t getHeightStride() const;
	int32_t getBufferCount() const;
	status_t getStatus() const;
private:
	virtual ~Overlay();
	sp<OverlayRef> mOverlayRef;
	overlay_data_device_t *mOverlayData;
	status_t mStatus;
}；
```
在上述代码中，通过surface来控制Overlay，其实也可以不使用Overlay的情况下统一进行管理。此处通过OverlayRef来创建Overlay的，一旦获取Overlay就可以通过Overlay获取到用来显示Address地址。向Address中写入数据后就可以显示我们的图像了。

### 4.实现Overlay硬件抽象层
在Android系统中，提供了一个Overlay硬件抽象层的框架实现。其中有完整的实现代码，可以将其使用Overlay硬件抽象层方法。但是其中没有使用具体的硬件，所以不会有实际的显示效果`hardware/libhardware/module/overlay/`

上述代码中包含Android.mk和Overlay.cpp

```makefile
LOCAL_PATH:=$(call my-dir)

#HAL module implementation,not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE)ID>.<ro.product.board>.so
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE:=false
LOCAL_MODULE_PATH:=(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES:=liblog
LOCAL_SRC_FILES:=overlay.cpp
LOCAL_MODULE:=overlay.trout
include $(BUILD_SHARED_LIBRARY)
```
overlay库是一个C语言库，没有其他库链接，在使用时是被动打开的。所以必须放置在目标文件系统的`system/lib/hw`目录中.overlay.cpp实现如下:

```cpp
//此结构体中扩充overlay_control_device_t结构体
struct overlay_control_context_t
{
	struct overlay_control_device_t device;
}；
//此结构体用于扩充overlay_data_device_t结构体
struct overlay_data_context_t
{
	struct overlay_data_devcie_t device;
};
//定义打开函数
static int overlay_device_open(const struct hw_module_t*module,const char*name,struct hw_deivce_t**device)
{
	static struct hw_module_methods_t overlay_module_methods={
		open:overlay_device_open
	};	
}
struct overlay_module_t HAL_MODULE_SYM={
	common:{
		tag:HARDWARE_MODULE_TAG,
		version_major:1,
		version_minor:0,
		id:	OVERLAY_HARDWARE_MODULE_ID,
		name:"sample overlay module",
		methods:&overlay_module_methods,
	}
};

static int overlay_device_open(const struct hw_module_t*module,const char*name,struct hw_device_t**device){
	int status=-EINVAL；
	if(!strcmp(name,OVERLAY_HARDWARE_CONTROL)){
		struct overlay_control_context_t *dev;
		dev=(overlay_control_context_t*)malloc(sizeof(*dev));
		memset(dev,0,sizeof(*dev));
		dev->device.common.tag=HARDWARE_DEVICE_TAG;
		dev->common.version=0;
		dev->common.module=const_cast<hw_module_t*>(module);
		dev->common.close=overlay_control_close;
		dev->device.get=overlay_get;
		dev->device.createOverlay=overlay_createOverlay；
		dev->device.destroyOverlay=overlay_destroyOverlay;
		dev->device.setPostion=overlay_setPosition;
		dev->device.getPosion=overlay_getPositon;
		dev->devcie.setParameter=overlay_setParameter;
		*device=&dev->device.common;
		status=0;
	}else if(!strcmp(name,OVERLAY_HARDWARE_DATA)){
		struct overlay_data_context_t *dev;
		dev=(overlay_data_context_t*)malloc(sizeof(*dev));
		memset(dev,0,sizeof(*dev));
		dev->device.common.tag=HARDWRE_DEVICE_TAG;
		dev->device.common.version=0;
		dev->device.common.module=const_cast<hw_moduel_t*>(module);
		dev->device.common.clsoe=overlay_data_close;
		dev->device.initialize=overlay_dequeueBuffer;
		dev->device.dequeuBuffer=overlay_dequeueBuffer;
		dev->device.queuqBuffer=overlay_queueBuffer;
		dev->device.getBufferAddress=overlay_getBufferAddress;
		*device=&dev->device.common;
		status=0;
	}
	return status;
}
```
