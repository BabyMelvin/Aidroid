# stagefright引擎
内置OpenCore最大特点就是跨平台。有较好的可移植性，并且已经通过多方验证，相对比较稳定。但是过于庞大，需要消耗较多时间去维护，为了解决这个问题，Android 2.0开始，Android引入架构稍微简单的框架-stagefright。

## 1.Stagefright代码结构：
Stagefright是一个轻量级的多媒体框架，其主要基于OpenMax实现的。在Stagefright中提供了媒体播放等接口，这些接口可以为Android框架层所使用。

在Android开源代码中

* Stagefright头文件路径如下：`frameworks/base/include/media/stagefright/`
* 源文件路径：`frameworks/base/media/libstagefright`
* 实现Stagefright播放器和录音器功能的文件路径：`frameworks/base/media/libmediaplayerservice`
* 测试Stagefright功能代码路径如下：`frameworks/base/cmds/stagefright/`

## 2.Stagefright实现OpenMax接口
Stagefright可以实现Android 系统中OpenMax接口，可以让Stagefright引擎内的OMXCode调用实现OpenMax接口，最终牡蛎是使用OpenMax IL 编/解码功能。

由此可见，在Android中是通过Stagefright来定义OpenMax接口的，具体实现内容保存在omx目录中。在头文件`media/libstagefright/include/OMX.h`中实现了Android标准的IOMX类，此文件主要代码：

```cpp
class OMX:public BnOMX,public IBinder::DeathRecepient{
public:
	OMX();
	virtual bool livesLocally(pid_t pid);
	virtual status_t listNodes(List<ComponentInfo>*list);
	virtual status_t allocateNode(const char*name,const sp<IOMXObserver>*observer,node_id*node);
	virtual status_t freeNode(node_id node);
	virtual status_t sendCommand(node_id node,OMX_COMMANDTYPE cmd,OMX_s32 params);
	virtual status_t getParameter(node_id node,OMX_INDEXTYPE index,void*params,size_t size);
	virtual status_t emptyBuffer(
	node_id node,buffer_id buffer,
	OMX_U32 range_offset,OMX_U32 range_length,
	OMX_U32 flags,OMX_TICKS timestamp);
	virtual status_t getExtensionIndex(
	node_id node,const char*parameter_name,
	OMX_INDEXTYPE* index);
	virtual sp<IOMXRender> createRender(
	const sp<ISurface> &surface,
	const char*componentName,
	OMX_COLOR_FORMATTYPE colorFormat,
	size_t encodedWidth,size_t encodedHeight,
	size_t displayWidth,size_t displayHeight,
	int32_t rotationDegrees);
};
```

文件`frameworks/base/media/libstagefright/omx/OMX.cpp`是上述OMX.h文件的实现，定义函数createRender()来创建映射，首先建立一个`hardware renderer`-SharedVideoRender(libstagefrighthw.so)。如果失败则建立`software render`-SoftwareRender(surface)。此函数的主要实现：

```cpp
sp<IOMXRenderer> OMX::createRender(const sp<ISurface> &surface,
const char*componentName,
OMX_COLOR_FORMATTYPE colorFormat,
size_t encodedWidth,size_t encodedHeight,
size_t displayWidth,size_t displayHeight,
int32_t rotationDegrees){
	Mutex::Autolock autoLock(mLock);
	VideoRender* impl=NULL;
	void* libHandle=dlopen("libstagefrighthw.so",RTLD_NOW);
	if(libHandle){
		typedef VideoRenderer*(*CreateRendererWithRotationFunc)(
		const sp<ISurface> &surface,
		const char* componentName,
		OMX_COLOR_FORMATTYPE colorFormat,
		size_t displayWidth,size_t displayHeight,
		size_t decodedWidth,size_t decodedHeight,
		int32_t rotationDegress);
		CreateRendererWithRotationFunc funcWithRotation=
		(CreateRendererWithRotationRunc)dlsym(
		libHandle,
		"_Z26createRendererWithRotationRKN7android2spINS_8"
	"ISurfaceEEEPKc200OMX_COLOR_FORMATTYPEjjjjji");
		if(funcWithRotation){
			impl=(*funcWithRotation)(surface,compnentName,colorFormat,displayWidth,displayHeight,encodedWidth,encodeedHeight,rotationDegrees);
		}else{
			CreateRendererFunc func=(CreateRendererFunc)
			dlsym(
			libHandle,
			"_Z26createRendererWithRotationRKN7android2spINS_8"
	"ISurfaceEEEPKc200OMX_COLOR_FORMATTYPEjjjjji");
		}
		if(impl){
			impl=new SharedVideoRender(libhandle,impl);
		libHandle=NULL;
		}
		if(libHandle){
			dlclose(libHandle);
			libHandle=NULL;
		}
		if(!impl){
			LOGW("Using software renderer");
			impl=new SoftwareRenderer(colorFormat,surface,displayWidth,displayHeight,
			encodedWidth,encodedHeight);
			if(((SoftwareRenderer*)impl)->initCheck()!=OK){
				delete impl;
				impl=NULL;
				return NULL;
			}
		}
	}
	return new OMXRender(impl);
}
```

由此可见，OMXMaster是OMX.cpp真正实现者，并且能够管理OpenMax插件类，这些通过头文件OMXMaster.h和源代码文件OMXMaster.cpp实现的，其中文件`frameworks/base/include/media/stagefright/OMXMaster.h`中定义了OMXMaster:

```cpp
struct OMXCodec:public MeidaSource,public MediaBufferObserver{
	enum CreationFlags{
		kPreferSoftwareCodecs=1,
		kIgnoreCodecSpecificData=2,
		kClientNeedsFrameBuffer=4,
	};
	static sp<MediaSource> Create(
		const sp<IOMX> &omx;
		const sp<MetaData> &meta,bool createEncoder,
		const sp<MediaSource> &source,
		const char*matchComponentName=NULL,
		uint32_t flags=0
	);
	static void setComponentRole{
		const sp<IOMX> &omx,IOMX::node_id node,bool isEncoder,
		const char*mime
	};
	virtual status_t start(MetaData*params=NULL);
	virtual status_t stop();
	virtual sp<MetaData> getFormat();
};
```
在文件`frameworks/base/media/libstagefright/OMXMaster.cpp`中，定义静态函数Create()将MediaSource作为IMOX插件给OMXCode。函数Create()如下：

```cpp
sp<MediaSource> OMXCodec::Create(
const sp<IOMX> &omx,
const sp<MataData> &meta,bool createEncoder,
const sp<MediaSource>&source,
const char*matchComponentName,uint32_t flags){
	const char*mime;
	bool success=meta->findCString(kKeyMIMEType,&mime);
	CHECK(success);
	Vector<String8> matchingCodecs;
	findMatchingCodecs(mime,createEncoder,matchComonentName,flags,&matchingCodecs);
	if(matchingCodecs.isEmpty()){
		return NULL;
	}
	sp<OMXCodecObserver> observer=new OMXCodecOberser;
	IOMX::node_id node=0;
	const char*componentName;
	//寻找插件
	for(size_t i=0;i<matchingCodecs.size();++i){
		componentName=matchingCodecs[i].string();
		sp<MediaSource> softwareCodec=createEncoder?InstantiateSoftwareEncoder(componentName,source,meta):
		InstatiateSoftwareCodec(componentName,source);
		if(softwareCodec!=NULL){
			LOGV("Successfully allocated software codec '%s'gn",componentName);
			return softwareCodec;
		}
		LOGV("Attemping to allocate OMX node %s",componentName);
		uint32_t quirks=getComponentQuirks(componentName,createEncodec);
		if(!createEncoder&&(quirks&kOutputBuffersAreUnreachable)&&(flags&kClientNeedsFramebuffer)){
			if(strncmp(componentName,"OMX.SEC.",8)){
				LOGW("Component %s does not give the client access to the framebuffer continues skipping",componentName);
				continue;
			}
		}
		status_t err=omx->allocateNode(componentName,observer,&node);
		if(err==OK){
			LOGV("Successfully allocated OMX node %s",componentName);
			sp<OMXCodec> codec=new OMXCodec(omx,node,quirks,createEncoder,mime,componentName,source);
			observer->setCodec(codec);
			err=codec->configureCodec(meta,flags);
			if(err==OK){
				return codec;
			}
			LOGV("Failed to configure codec %s",componentName);
		}
	}
	return NULL;
}
```

## 3.Video Buffer传输流程
在Stagefight引擎中，播放的流程是处理Video Buffer的过程。在Stagefright中需要使用VideoRenderer插件来处理Video Buffer。接下来将简要讲解使用插件Video Buffer流程。

* 1.一开始，OMXCodec会通过函数read()来传送未解码数据给decoder，并要求decorder回传解码后的数据。

```cpp
status_t OMXCOdec::read(...)
{
	if(mInitialBufferSubmit){
		mInitialBufferSubmit=false;
		drainInputBuffers();//<-OMX_EmptyThisBuffer
		fillOutputBuffers();//<-OMX_FillThisBuffer
	}
	...
}
void OMXCodec::drainInputBuffers()
{
	Vector<BufferInfo>*buffer=&mPortBuffers[kPortIndexInput];
	for(i=0;i<buffers->size();++i)
	{
		drainInputBuffer(&buffers->editItemAt(i));
	}
}
void OMXCodec::drainInputBuffer(BufferInfo*info)
{
	mOMX->emptyBuffer{...};
}
void OMXCodec::fillOutputBuffers()
{
	Vector<BufferInfo> *buffers=mPortBuffers[kPortIndexOutput];
	for(i=0;i<buffers->size();++i){
		fillOutputBuffer(&buffers->editItemAt(i));
	}
}
void OMXCodec::fillOutputBuffer(BufferInfo*info)
{
	mOMX->fillBuffer{...};
}
```

* Decoder从`input port`(输入点)获取资料，然后进行解码处理，并且通过回传EmptyBufferDone的方式通知OMXCodec当前所进行的工作。对应：

```cpp
void OMXCodec::on_message(const omx_message& msg){
	switch(msg.type){
		case omx_message::EMPTY_BUFFER_DONE:
			IOMX::buffer_id buffer=msg.u.extended_buffer_data.buffer;
			drainInputBuffer(&buffers->editItemAt(i));
	}
}
```

* 3.当OMXCodec接收到EMPTY_BUFFER_DONE后，继续传下一个未解码的资料给decoder。Decorder解码后的资料送到`output port`(输出点)，并且通过回传FillBufferDone的方式通知OMXCodec:

```cpp
void OMXCodec::on_message(const omx_message &msg)
{
	switch(msg.type){
		IOMX::buffer_id buffer=msg.u.extended_buffer_data.buffer;
		fillOutputBuffer(info);
		mFilledBuffers.push_back();
		mBufferFilled.signal();
	}
}
```
当OMXCodec收到FILL_BUFFER_DONE后，将解码后的资料放入mFilledBuffers,然后发出mBufferFilled信号，并要求decoder继续发出资料。

* 4.使用函数read()等待mBufferFilled信号，当mFilledBuffers被填入资料后，函数`read()`将其指定给buffer,并回传AwesomePlayer：

```cpp
status_t OMXCodec::read(MediaBuffer **buffer,...)
{
	。。。
	while(mFilledBuffers.empty())
	{
		mBufferFilled.wait(mLock);
	}
	BufferInfo*info=&mPortBuffers[kPortIndexOutput].editItemAt(index);
	info->mMediaBuffer->add_ref();
	*buffer=info->mMediaBuffer;
}
```

函数AwesomePlayer::onVideoEvent()除了通过OMXCodec::read去得到解码后的资料后，还需要将这些资料(mVideoBuffer)传递给`video renderer`,以便在屏幕上显示。

* 1.将mVideoBuffer中的资料输出之前，必须建立mVideoRenderer.

```cpp
void AwesomePlayer::onVideoEvent()
{
	...
	if(mVideoRenderer==NULL){
		initRenderer_l();
	}
	...
}
void AwesomePlayer::initRender_l()
{
	if(!strncmp("OMX.",component,4)
	{
		mVideoRenderer=new AwesomeRemoteRenderer(
		mClient.interface()->createRender(mISurface,component,...));
	}else{
		mVideoRenderer=new AwesomeLocalRender()
		...,compoenent,mISurface;
	}
}
```

* 2.如果`Video Decoder`是OMX component，则需要建立一个AwesomeRemoteRenderer作为mVideoRenderer。从步骤1中，AwesomeRemoteRenderer的本质是由函数OMX::createRenderer()创建的。函数createRender()先建立一个硬件渲染器-ShareVideoRenderer(在libstagefrighthw.so中定义)，如果失败则建立软件渲染器-SoftwareRenderer(使用surface接口实现)

```cpp
sp<IOMXRenderer> OMX::createRenderer(...){
	VideoRenderer*impl=NULL;
	libHandle=dlopen("libstagefrighthw.so",RTLD_NOW);
	if(libHandle){
		CreateRendererFunc func=dlsym(libHandle,...);
		impl=(*func)(...)//<-Hardwre Render
	}
	if(!impl){
		impl=new SoftwareRender(...);//<-Software Renderer
	}
}
``` 

* 4.建立mVideoRenderer后就可以开始将解码后的资料回传给它。

```cpp
void AwesomePlayer::onVideoEvent()
{
	if(!mVideoBuffer)
	{
		mVideoSource->read(&mVideoBuffer,...);
	}
	//[Check TimeStamp]
	if(mVideoRender==NULL){
		initRenderer_l();
	}
	mVideoRenderer->render(mVideoBuffer);//<-Render data
}
```
经过上述操作之后，Renderer的处理过程介绍万册灰完毕。在播放多媒体的时候，需要使用Audio来实现处理功能。在Stagefright框架中，Audio的部分内容是由AudioPlayer来处理的，此功能在函数AwesomePlayer::play_l()中建立。接下来使用Audio基本流程：

* 1.当要求播放影音时，会同时建立并启动AudioPlayer。对应代码

```cpp
status_t AwesomePlayer::play_l()
{
	...
	mAudioPlayer=new AudioPlayer(mAudioSink,...);
	mAudioPlayer->start(...);
	...
}
```

* 2.启动AudioPlayer过程中先读取第一笔解码后的资料，并开启Audio Output。对应的代码：

```cpp
status_t AudioPlayer::start(...)
{
	mSource->read(&mFirstBuffer);
	if(mAudioSink.get()!=NULL)
	{
		mAudioSink->open(...,&AudioPlayer::Audio);
		mAudioSink->start();
	}else{
		mAudioTrack=new AudioTrack(...,&AudioPlayer::AudioCallback,...);
		mAudioTrack->start();
	}
}
```
上述代码中，AudioPlayer并没有价格mFirstBuffer传递给Audio Output

* 3.在开启Audio Output的同时,AudioPlayer将启动函数callback(),这样每当函数callback()将被呼叫AudioPlayer时，会从AudioDecoder(音频解码器)读取解码后的资料：

```cpp
size_t AudioPlayer::AudioSinkCallback(audioSink,buffer,size,...)
{
	return fillBuffer(buffer,size);
}
void AudioPlayer::AudioCallback(...,info)
{
	buffer=info;
	fillBuffer(buffer->raw,buffer->size);
}
size_t AudioPlayer::fillBuffer(data,size)
{
	mSource->read(&mInputBuffer,...);
	memcpy(data,mInputBuffer->data(),...);
}
```
上述代码可以知道，读取解码后的音频资料工作是由函数`callback()`所驱动的，函数`fillBuffer()`会将资料复制到数据中，复制到音频输出会回头取用这些数据。
