# 6.系统层调用Overlay HAL 架构
## 6.1 调用Overlay HAL结构的流程
### 1.1 测试文件
下面提供了简单调用Overlay方法.`frameworks/base/libs/surfaceflinger/tests/overlays/overlayers.cpp`.

```cpp
itn main(int argc,char**argv)
{
	//建立线程池
	sp<ProcessState> proc(ProcessState::self());
	ProcessState::self()->startThreadPool();
	
	//创建一个SurfaceFlinger：client
	sp<SurfaceComposerClient> client=new SurfaceComposerClient();
	//创建一个Surface,最后那个参数是类型
	sp<Surface> surface=client->createSurface(getpid(),0,320,240,PIXEL_FORAMT_UNKNOWN,ISurfaceComposer::ePushBuffers);
	
	//取得Isurface接口
	sp<ISurface> isurface=Test::getISurface(surface);
	printf("isurface=%p\n",isurface.get());
	
	//创建一个overlay
	sp<OverlayRef>ref=isurface->createOverlay(320,240,PIXEL_FORMAT_RGB_565);
	sp<Overlay> overlay=new Overlay(ref);
	//创建好Overlay后，即可使用overlay的API，这些都对应到Overlay HAL 具体实现
	overlay_buffer_t buffer;
	overlay->dequeueBuffer(&buffer);
	printf("buffer=%p\n",buffer);
	void*address=overlay->getBufferAddress(buffer);
	printf("address=%p/n",address);
	最重要操作是通过queueBuffer将buffer列队
	overlay->queueBuffer(buffer);
	return 0;
}
```

### 1.2 在Android系统中创建Overlay
Overlay系统是一个功能强大的系统，不仅仅是简单实现试频输出功能，而且是哪了和摄像头、GPS等有关的功能。Overlay的具体应用主要体现在如下几个方面：

* 1.摄像头应用的实现文件：CameraService.cpp(`frameworks/base/camera/libcameraserver`):

```java
setPreviewDisplay()
startPreviewMode()
setOverlay()
createOverlay()
```

* 2.界面相关应用文件`ISurface.cpp(frameworks/base/libs/ui)`

LayerBaseClient::Surface::onTransact()函数位于LayerBase.cpp中，好像用于ibind进程通信的函数。其中函数BnSurface::onTransact()有5种方式只有确定有overlay硬件支持时才会调用`case CREATE_OVERLAY`语句.

```cpp
switch(code){
	case REQUEST_BUFFER:
		CHECK_INTERAFCE(ISurface,data,reply);
		int bufferidx=data.readInt32();
		int usage=data.readInt32();
		sp<GraphicBuffer> buffer(requestBuffer(bufferidx,usages));
		return GraphicBuffer::writeToParcel(reply,buffer.get());
	case REGISTER_BUFFER:
		CHECK_INTERACE(ISurface,data,reply);
		buffer.w=data.readInt32();
		buffer.h=data.readInt32();
		buffer.hor_stride=data.readInt32();
		buffer.ver_stride=data.readInt32();
		buffer.format=data.readInt32();
		buffer.transform=data.readInt32();
		buffer.flags=data.readInt32();
		buffer.heap=interface_cast<IMemeoryHeaP>(data.readStrongBinder());
		status_t err=registerBuffers(buffer);
		reply->writeInt32(err);
		return NO_ERROR;
	case UNREGISTER_BUFFER:
		CHECK_INTERACE(ISurface,data,reply);
		ssize_t offset=data.readInt32();
		unregisterBuffers();
		return NO_ERRO;
	case POST_BUFFER:
		CHECK_INTERACE(ISurface,data,reply);
		ssize_t offset=data.readInt32();
		postBuffer(offset);
		return NO_ERRO;
	case CREATE_OVERLAY:
		CHECK_INTERFACE(ISurface,data,reply);
		int w= data.readInt32();
		int h= data.readInt32();
		int f=data.readInt32();
		sp<OverlayRef> o=createOverlay(w,h,f);
		return OverlayRef::writeToParcel(replay,o);
	default:
		return BBinder::onTransact(code,data,reply,flags);
}
```

* 文件LayerBuffer.cpp(`frameworks/base/libs/surfaceflinger`)，此文件是createOverlay的实现：

```cpp
sp<OverlayRef> LayerBuffer::SurfaceLayerBuffer::createOverlay(uint32_t w,uint32_t h,int32_t format){
	//通过OverlaySource来创建overlay
	sp<OverlaySource> source =new OverlaySource(*this,&result,w,h,format);
}
//此函数调用了OverlayHAL的API createOverlay
LayerBuffer::OverlaySource::OverlaySource()
{
	overlay_control_device_t*overlay_dev=mLayer.mFlinger->getOverlayEngine();
	overlay_t*overlay=overlay_dev->createOverlay(overlay_dev,w,h,format);
	overlay_dev->setPararmeter(overlay_dev,overlay,OVERLAY_DITHER,OVERLAY_ENABLE);
	//设置参数，初始化OverlayRef类，OverlayRef的构造函数Overlay.cpp中
	mOverlay=overlay;
	mWidth=overlay->w;
	mHeight=overlay->h;
	mFormat=overlay->format;
	mWidthStride=overlay->w_stride;
	mHeightStride=overlay->h_stride;
	mInitialized=false;
	
	*overlayRef=new OverlayRef(mOverlayHandle,channel,mWidth,mHeiht,mFormat,mWidthStride,mHeightStride);
}
```

* 3.管理Overlay HAL 模块
文件Overlay.cpp(`frameworks/base/libs/ui`)功能是管理OverlayHAL模块，并封装HAL的API

* 3.1 打开Overlay HAL 模块

```cpp
Overlay::Overlay(const sp<OverlayRef>& overlayRef):mOverlayRef(overlayRef),mOverlayData(0),mStatus(NO_INIT)
{
	mOverlayData=NULL;
	hw_module_t const* module;
	if(overlayRef!=0){
		if(hw_get_module(MODULE_HARDWARE_MODULE_ID,&module)==0){
			if(overlay_data_open(module,&mOverlayData)==NO_ERROR){
				mStatus=mOverlayData->initialize(mOverlayData,overlayRef->mOverlayHandle);
			}
		}
	}
}
```
需要封装很多需要的API,比如T1自己编写的函数opencore()用于负责视频输出。各个API的实现和编写。

Overlay输出对象两种:

* 一种视频（YUV格式，系统的V4L2）
* 一种ISurface一些图像（RGB，直接写FrameBuffer）.Skeleton
