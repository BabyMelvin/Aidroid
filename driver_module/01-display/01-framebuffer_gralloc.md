# 1.显示系统介绍
Android显示系统的功能是操作显示设备并获得显示终端。显示系统对应于硬件层LCD、LCD控制器和VGA输出设备等显示设备。

显示系统驱动应用和Android的Surface库有着很多联系，显示系统下层实现了对基本输出的封装。Surface中有一部分提供了多个层的支持。

## 1.1 Android的版本
在Linux系统中，FrameBuffer驱动是标准的显示设备的驱动。

* PC来说，FrameBuffer是显卡驱动。
* 嵌入式SoC处理器，FrameBuffer通常是LCD控制器或者其他显示设备驱动。
* Donut（1.5）和Eclair（2.0）不同的Android版本：
	* 1.5版本及之前，使用`libui`直接调用FrameBuffer驱动程序来实现显示部分。
	* 2.0之后，增加Gralloc模块，**它位于显示设备和libui库中间的一个硬件模块**。

2.0以后版本，Gralloc(图形分配），作为显示系统的硬件抽象层来使用。Gralloc模块作为显示系统的硬件抽象层来使用。Gralloc模块保存在`/system/lib/hw`运行过程中使用`dlopen`和`dlsym`方式动态打开并取出符号来使用，系统其它部分没有链接此动态库。该模块可移植的，是系统和显示之间的接口，是以硬件的形式存在。在Android系统中，既可以使用FrameBuffer作为Gralloc模块的驱动程序，也可以在此模块中不使用FrameBuffer驱动。

## 1.2移植和调试前的准备
### 1.2.1 FrameBuffer驱动程序
`FrameBuffer`一个字符设备文件，通常对应设备节点`/dev/fbx`.FrameBuffer驱动主设备号是29,次设备号用递增数字生成。每个系统可以有多个显示设备,分别使用`/dev/fb0`、`/dev/fb1`和`/dev/fb2`等表示。

用户空间，通常使用`ioctl`,`mmap`等文件系统接口进行操作。

* `ioctl`用于获得和设置信息。
* `mmap`可以将FrameBuffer内存映射到用户空间。
* `write`直接用写凡是输出显示内容。

FrameBuffer机构图如下：

<image src="image/01-01.png"/>


FrameBuffer驱动主要涉及如下两个文件:

* `include/linux/fb.h`：驱动头文件
* `drivers/video/fbmem.c`：核心实现文件。

```c
struct fb_info{
	atomic_t count;
	int node;
	int flags;
	struct mutex lock; //lock for open / release /ioctl funcs
	struct mutex mm_lock; // lock for fb_mmap and smem_* filels
	//对应应用层ioctl FBIOGET_VSCREENINFO
	struct fb_var_screeninfo var;//显示屏的信息
	//对应于FBIOGET_FSCREENINFO
	struct fb_fix_screeninfo fix;//显示屏固定信息
	struct fb_monspecs monspecs;//当前显示器specs
	struct work_struct queue;//FrameBuffer事件队列
	struct fb_pixmap pixmap;//图像硬件制图
	struct fb_pixmap sprite;//游戏硬件制图
	struct fb_cmap cmap    ;//当前cmap
	struct list_head modelist;//模式列表
	struct fb_videomode * mode;//当前模式
    struct fb_ops* fbops;
    struct device* device;//this is the parent
    struct device* dev;   //this is the fb device
#define FBINFO_STATE_RUNNING   0
#define FBINFO_STATE_SUSPENDED 1
    u32 state;   //hardwaare state
    void* fbcon_par;  //fbcon use-only private area

};
```
`ioctl`命令也在fb.h中定义:

```c
#define FBIOGET_VSCERRNINFO 0x4600 //获得屏幕变化信息
#define FBIOPUT_VSCREENINFO 0X4601 //设置变化屏幕信息
#define FBIOGET_FSCREENINFO 0x4602 //获得固定屏幕信息
#define FBIOGETCAMP         0x4604 //获得映射取内容
#define FBIOPUTCMAP         0x4605 //设置映射区内容
#define FBIOPAN_DISPLAY     0x4606 //调整显示区域
```

## 1.3硬件抽象层
### 1.3.1 版本1.5之前
显示系统的硬件抽象层位于libui标准库，不需要移植这部分内容。整个只需要移植FrameBuffer驱动程序即可。又因为libui对于FrameBuffer驱动程序使用了标准规则，只要当前系统实现了framebuffer驱动程序，就能够在1.5之前实现显示，区别只是路径。

1.5之前版本硬件抽象层的核心文件`frameworks/base/libui/ui/EGLDisplaySurface.cpp`,核心功能通过mapFrameBuffer()实现的。

```c
status_t EGLDisplaySurface::mapFrameBuffer()
{
	char const*const device_template[]={
		"dev/graphics/fb%u",
		"dev/fb%u",
		0
	};
	int fd=-1;
	int i=0;
	char name[64];
	while((fd==-1)&&device_template[i]){
		snprintf(name,64,device_template[i],0);
		fd=open(name,O_RDWR,0);
		i++;
	}
	if(fd<0){
		return -errno;
	}
	struct fb_fix_screeninfo finfo;
	if(ioctl(fd,FBIOGET_FSCREENINFO,%&finfo)==-1{
		return -errno;
	}
	struct fb_var_screeninfo info;
	if(ioctl(fb,FBIOCGET_VSCREENINFO,&info)==-1){
		return -errno;
	}
	info.reserved[0]=0;
	info.reserved[1]=1;
	info.reserved[2]=2;
	info.xoffset=0;
	info.yoffset=0;
	info.yres_virtual=info.yres*2;
	info.bits_per_pixel=16;
	//明确请求5/6/5
	info.red.offset=11;
	info.red.length=5;
	info.green.offset=5;
	info.green.length=6;
	info.blue.offset=0;
	info.blue.length=5;
	info.transp.offset=0;
	info.transp.lenght=0;
	info.activate=FB_ACTIVATE_NOW;
	uint32_t flags=PAGE_FLIP;
	if(ioctl(fd,FBIOPUT_VSCREENINFO,&info)==-1){
		info.yres_virtual=info.yres;
		flag&=~PAGE_FLIP;
		LOGW("FBIOPUT_VSCREENIFNO failed,page flipping not support");
	}
	if(info.yres_virtual<info.yres*2){
		info.yesl_virtual=info.yres;
		flag&=~PAGE_FLIP;
		LOGW("page flipping not support (yres_virtual=%d,requested=%d)",info.yres_virtual,info.yres*2);
	}
	if(ioctl(fd,FBIOGET_VSCREENINFO,&info)==-1)
		return -errno;
	int refreshRate=1000000000000LLU/
	(
		uint64_t(info.upper_margin+info.lower_margin+info.yres)
		*(info.left_margin+info.right_margin+info.xres)
		*info.pixclock
	);
	if(refreshRate==0){
		//bleagh,bad info from the driver
		refreshRate=60*1000; //69HZ
	}
	if(int(info.width)<=0||int(info.length)<=0){
		//the driver doesn't return that information
		//default to 160 dpi
		info.width=51;
		info.height=38;
	}
	float xdpi=(info.xres*25.4f)/info.width;
	float ydpi=(info.yres*25.4f)/info.height;
	float fps=refreshRate/1000.0f;
	LOGI("using (fd=%d)\n"
		"id =%s\n"
		"xres=%d px\n"
		"yres=%d px\n"
		"xres_virtual=%d\n"
		"yres_virtual=%d\n"
		"bpp         =%d\n"
		"r           =%2u:%u\n"
		"g           =%2u:%u\n"
		"b           =%2u:%u\n",
		fd,
		finfo.id,
		info.xres,
		info.yres,
		info.xres_virtual,
		info.yres_virtual,
		info.bits_per_pixel,
		info.red.offset,info.red.length,
	info.green.offset,info.green.lenght,
	info.blue.offset,info.blue.lenght
	);
	LOGI(
		"width   =%d mm (%f dpi)\n"
		"height  =%d mm (%f dpi)\n"
		"refresh rate =%2.f Hz\n",
		info.width,xdpi,
		info.height,ydpi,
		fps
	);

	if(ioctl(fd,FBIOGET_FSCREENINFO,&finfo)==-1){
		return -errno;
	}
	if(finfo.smem_len<0)
		return -errno;
	//打开并且映射显示
	void*buffer =(uint16_t*)mmap(
	0,finfo.smem_len,
	PROT_READ|PROT_WRITE,
	MAP_SHARED,fd,0);
	if(buffer==MAP_FAILED){
		return -errno;
	}
	//暂时清除fb
	memset(buffer,0,finfo.mmem_len);
	uint8_t *offscreen[2];
	offscreen[0]=(uint8_t*)buffer;
	if(flags& PAGE_FLIP){
		offscreen[1]=(uint8_t*)buffer+finfo.line_length*info.yres;
	}else{
		offscreen[1]=(uint8_t*)malloc(finfo.smem_len);
		if(offscreen[1]=0){
			munmap(buffer,finfo.smem_len);
			return NO_MEMORY;
		}
	}
	mFlags=flags;
	mInfo=info;
	mFInfo=Finfo;
	mSize=finfo.smem_len;
	mIndex=0;
	for(int i=0;i<2;i++){
		mFb[i].version=sizeof(GGLSurface);
		mFb[i].width=info.xres;
		mFb[i].height=info.yres;
		mFb[i].stride=finfo.line_length/(info.bits_per_pixel>>3);
		mFb[i].data=(GGLubyte*)(offscreen[i]);
		mFb[i].format=GGL_PIXEL_FORMAT_RGB_565;
	}
	return fd;
}
```
<image src="image/01-02.png"/>

### 1.3.2 版本2.0之后
2.0之后，Gralloc模块用于显示部分硬件抽象层。因为Gralloc模块灵活，移植方式也是多种多样，具体分为如下两种:

* 如果继续使用Android中已实现的Gralloc模块，就可以继续使用标准FrameBuffer驱动程序，只需要移植FrameBuffer内容就可以。
* 如果想要自己实现特定Gralloc模块，次模块就是当前系统的显示设备和Android接口，此时显示设备可以是各种类型的驱动程序。

**注意**：对一个标准FrameBuffer驱动程序优化改动，需要额外增加一些ioctl命令来获取额外的控制。例如Android的pmem驱动程序可以实现获取加速效果。

#### 1.3.2.1 头文件
Gralloc模块的头文件在`hardware/libhardware/include/hardware/gralloc.h`中定义。

1.定义子设备和模块的名称：

```c
//模块名称
#define GRALLOC_HARDWARE_MODULE_ID "gralloc"
//fb0是FrameBuffer设备
#define GRALLOC_HARDWARE_FB0 "fb0"
//gpu0是图形处理单元设备
#define GRALLOC_HARDWARE_GPU0 "gpu0"
```

2.通过扩展定义`gralloc_module_t`实现Gralloc硬件模块：

```c
typedef struct gralloc_module_t{
	struct hw_module_t common;
	//在alloc_devce_t::alloc前调用
	int(*registerBuffer)(struct gralloc_module_t const*module,buffer_handle_t handle);
	int (*unregisterBuffer)(struct gralloc_module_t const*module,buffer_handle_t handle);
	//用于访问特定缓冲区，在调用此接口时硬件设备需要结束渲染或完成同步处理
	int (*lock)(struct gralloc_module_t const*module,buffer_handle_t handle,
	int usage,int l,int t,int w,int h,void**vaddr);
	//在所有buffer改变之后调用
	int (*unlock)(struct gralloc_module_t const*module,int operate,...);
	//未来保留
	void* reserved_proc[7];
}gralloc_module_t;
```
 
3.定义函数gralloc_open()，功能打开gralloc接口

```c
static inline int gralloc_open(const struct hw_module_t*module,struct alloc_device_t**device){
	return module->methods->open(module,GRALLOC_HARDWARE_GPU0,(struct hw_device_t**)device)
}
```

4.定义函数frameBuffer_open()，功能打开FrameBuffer的接口：

```c
static inline int framebuffer_open(const struct hw_module_t**module,struct framebuffer_device**device){
	return module->methods->open(module,GRALLOC_HADWARE_FB0,(struct hw_device_t**)device);
}
```

5.定义函数framebuffer_close，定义关闭FrameBuffer接口。

```c
static inline int framebuffer_close(struct framebuffer_device*device){
	return device->common.close(&device->common);
}
```

6.定义函数gralloc_close()函数，功能关闭gralloc接口

```c
static inline int gralloc_close(struct alloc_device_t*device){
	return device->common.close(&device->commone);
}
```

7.设备GRALLOC_HARDWARE_GPU0对应的结构体`alloc_device_t`.设备GRALLOC_HARDWARE_FB0对应结构体`framebuffer_device_t`具体定义：

```c
typedef struct alloc_device_t{
	struct hw_device_t common;
	int(*alloc)(struct alloc_device_t*dev,//以宽，高，颜色格式为参数分配
	int w, int w,int format,int usage,
	buffer_handle_t*handle,int*stride);
	int (*free)(struct alloc_device_t*dev,
	buffer_handle_t handle);
}alloc_device_t;

typedef struct framebuffer_device_t {
	struct hw_device_t common;
	const uint32_t flags;
	const uint32_t width;//宽
	const uint32_t height;//高
	const int      stride;//每行内容
	cosnt int      format;//颜色格式
	const float    xdpi;  //x方向像素密度
	const float    ydpi;//y方向像素密度
	const float    fps; //帧率
	const int      minSwapInterval;
	const int      maxSwapInterval;
	int reserved[8];
	int (*setSwapInterval)(struct framebuffer_device_t*window,int interval);
	int (*setUpdateRect)(struct framebuffer_device_t*window,
	int left,int top,int width,int height);
	int (*post)(struct framebuffer_device_t*dev,buffer_handle_t buffer);
	int (*compositionComplete)(struct framebuffer_device_t*dev);
	void*reserved_proc[8];
}framebuffer_device_t;
```

#### 1.3.2.2 源文件
Gralloc模块是由gralloc_module_t模块、alloc_device_t设备和framebuffer_deivce_t设备这三个结构体描述，里面指针起了非常重要作用。Gralloc模块是由ui库中文件调用:

```
frameworks/base/include/ui/FramebufferNativeWindow.cpp
```
文件中`FrameBufferNativeWindow.cpp`定义了类FrambufferNativeWindow,此类继承了`android_native_buffer_t`,这是对上层接口，表示这个一个本地窗口。

1. 定义构造函数FramebufferNativeWindow()实现如下：

```c
FramebufferNativeWindow::FramebufferNativeWindow():BASE():fbDev(0),grDev(0),mUpdateOnDemand(false){
	hw_module_t const *module;
	if(hw_get_module(GRALLOC_HARDWARE_MODULE_ID，&module)==0){
		int stride;
		int err;
		err=framebuffer_open(module,&fbDev);//打开FrameBuffer设备
		LOGE_IF(err,"couldn't open framebuffer HAL (%s)",strerror(-err));	
		err=gralloc_open(module,&grDev);//打开Gralloc设备,
		//bail out if we can't initialize the modules
		if(!fbDev||!grDev){
			return;
		}
		mUpdateedOnDemand=(fbDev->setUpdateRect!=0);
		//初始化 buffer FIFO
		mNumBuffers=2;
		mNumFreeBuffers=2;
		mBufferHead=mNumBuffers-1;
		//初始化两个缓冲区
		buffers[0]=new NativeBuffer(fbDev->width,fbDev->height,fbDev->format,GRALLOC_USAGE_HW_FB);
		bufferes[1]=new NativeBuffer(fbDev->width,fbDev->height,fbDev->format,GRALLOC_USAGE_HW_FB);
		err=grDev->alloc(grDev,fbDev->width,fbDev->height,fbDev->format,GRALLOC_USAGE_HW_FB,&buffers[0]->handle,&buffers[0]->stride);
		LOGE_IF(err,"fb buffer 0 allocation failed w=%d,h=%d,err=%s",fbDev->width,fbDev->height,strerror(-err));
		//从Gralloc设备中分配内存
		err=grDev->alloc(grDev,fdDev->width,fbDev->height,strerror(-err));
		//从FrameBuffer设备中获得常量
		const_cast<uint32_t&>(android_native_window_t::flags)=fbDev->flags;
		const_cast<float&>(android_native_window_t::xdpi)=fbDev->xdpi;
		const_cast<int&>(android_native_window_t::ydpi)=fbDev->ydpi;
		const_cast<int&>(android_native_window_t::minSwapInterval)=fbDev->minSwapInterval;
		const_cast<int&>(android_native_window_t::maxSwapInterval)=fbDev->maxSwapInterval;
	}else{
		LOGE("couldn't get gralloc module");
	}
	//赋值各个处理函数的指针
	android_native_window_t::setSwapInterval=setSwapInterval;
	android_native_window_t::dequeueBuffer=dequeueBuffer;	
	android_native_window_t::lockBuffer=lockBuffer;
	android_native_window_t::queueBuffer=ququeBuffer;
	android_native_window_t::query=query;
	android_native_window_t::perform=perform;
}
```
在FramebufferNativeWindow()使用了双显示区缓冲方式。

#### 1.3.4 文件`GraphicBufferAlloctor.cpp`

文件`framework/base/libs/ui/GraphicBufferAllocator.cpp`通过调用Gralloc模块和gralloc_module_t模块来显示缓冲区的分配：

```c
status_t GraphicBufferAllocator::alloc(uint32_t w,uint32_t h,PixelFormat format,int usage,buffer_handle_t &handle,int32_t*stride){
	//保证没分配0x0缓冲
	w=clamp(w);
	h=clamp(h);
	//有一个h/w分配者，并且h/w缓冲请求
	status_t err;
	if(usage&GRLLOC_USGAE_HW_MASK){
		//mAllocDev为alloc_device_t设备类型
		err=mAllocDev->alloc(mAllocDev,w,h,format,usage,handle,stride);
	}else{
		err=sw_gralloc_handle_t::alloc(w,h,format,usage,handle,stride);
	}
	LOGW_IF(err,"alloc(%u,%u,%d,%08x,...) failed %d (%s)",w,h,format,usage,err,strerror(-err));
	if(err==NO_ERRNOR){
		Mutex::Autolock _l(sLock);
		KeyedVecotr<buffer_handle_t,alloc_rec_t>& list(sAllocList);
		alloc_rect_t rec;
		rec.w=w;
		rec.h=h;
		rec.formt=format;
		rec.usage=usage;
		rec.vaddr=0;
		rec.size=h*stride[0]*bytesPerPixel(format);
		list.add(*handle,rec);
	}else{
		String8 s;
		dump(s);
		LOGD("%s",s.string());
	}
	return err;
}
```
### 1.3.4 文件GraphicBuferMapper.cpp
文件`frameworks/base/libs/ui/GraphicBufferMapper.cpp`通过调用Gralloc模块显示缓冲的映射，并且在注册了显示的缓冲内容。

```c
//注册显示的缓冲内容
status_t GraphicBufferMapper::registerBuffer(buffer_handle_t handle){
	status_t err;
	if(sw_gralloc_handle_t::validate(handle)<0){
		err=mAllocMod->registerBuffer(mAllocMode,handle);
	}else{
		err=sw_gralloc_handle_t::registerBuffer((sw_gralloc_handle_t*)handle);
	}
	LOGW_IF(err,"registerBuffer(%p) failed %d (%s)",handle,err,strerror(-err));
	return err;
}
//注销显示的缓冲内容
status_t GraphicBufferMapper::unregisterBuffer(buffer_handle_t handle){
	status_t err;
	if(sw_gralloc_handle_t::validate(handle)<0){
		err=mAllocMod->unregisterBuffer(mAllocMod,handle);
	}else{
		err=sw_gralloc_handle_t::unregisterBuffer((sw_gralloc_handle_t*)handle);
	}
	LOGW_IF(err,"unregisterBuffer(%p) failed %d (%s)",handle,err,strerror(-err));
	return err;
}
//锁定
status_t GraphicBufferMapper::lock
{
	status_t err;
	if(sw_gralloc_handle_t::validate(handle)<0){
		err=mAllocMode->lock(mAllocMode,handle,usage,bounds.left,bounds.top,bounds.width(),bounds.height(),vaddr);
	}else{
		err=sw_gralloc_handle_t::lock)sw_gralloc_handle_t*)handle,usage,bouds.left,bounds.top,bounds.width(),bounds.height(),vaddr);
	}
	LOGW_IF(err,"lock()... failed %d (%s)",err ,strerror(-err));
	return err;
}
//解锁
status_t GraphicBufferMapper::unlock(buffer_handle_t handle){
	status_t err;
	if(sw_gralloc_handle_t::validate(handle)<0){
		err=mAllocMod->unlock(mAllocMod,handle);
	}else{
		err=sw_gralloc_handle_t::unlock((sw_gralloc_handle_t*)handle);
	}
	LOGW_IF(err,"unlock(...) failed %d (%s)",err,strerror(-err));
	return err;
}
```
其中mAllocDev是一个`alloc_device_t`设备类型，根据句柄的范围可以从Gralloc模块中注册Buffer，也可以从软件中注册Buffer.

#### 1.3.5 文件`LayerBuffer.cpp`
在管理库SurfaceFlinger中也调用了Gralloc模块，调用路径如下:
```c
frameworks/base/libs/surfaceflinger/LayerBuffer.cpp
```
SurfaceFlinger主要功能如下:

* 将Layers(Surfaces)的内容刷新到屏幕。
* 维持Layer的Zorder序列，并对Layer最终输出做出裁剪计算。
* 响应Clinet要求，创建Layer与客服端的Surface建立连接。
* 接收Client要求，修改输出大小、Alpha等Layer属性。

对于SurfaceFlinger实际意义，首先要知道如何投递、投递物、投递路线和投递目的地。

<image src="image/01-03.jpg"/>

SurfaceFlinger的管理对象如下：

* mClientsMap:管理客服端与服务端的链接。
* ISurface,ISurfaceComposer:AIDL调用接口实例。
* mLayerMap:服务端的Surface的管理对象。
* mCurrentState.layersSortedByZ:以Surface的Z-order序列排序的Layer数组。
* graphicPlane:缓冲区输出管理。
* OpenGL ES:图形计算、图像合成等图形库。
* gralloc.xxx.so：这个跟平台相关的图形缓冲区管理器。
* pmem Device:提供共享内存。在这里只是在gralloc.xxx.so可见，在上层被gralloc.xxx.so抽象了。

在文件LayBuffer.cpp中定义的类是一个Buffer类，并为其定义了构造代码如下:

```cpp
LayerBuffer::Buffer::Buffer(const ISurface::BufferHeap& buffers,ssize_t offset):mBufferHeap(buffers){
	NativeBuffers&src(mNativeBuffer);
	src.crop.l=0;
	src.crop.t=0;
	src.crop.r=buffer.w;
	src.crop.b=buffer.h;
	src.img.w=buffers.hor_stride?:buffers.w;
	src.img.h=buffers.ver_stride?:buffers.h;
	src.img.format=buffers.format;
	src.img.offset=offset;
	src.img.base=buffers.heap->base();
	src.image.fd=buffers.heap->heapIO();
}
```
