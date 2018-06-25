image# 音频系统驱动
## 1.音频系统结构
Android音频系统对应硬件设备有音频输入设备和音频输出设备两部分.

* 手机中输入设备通常是话筒
* 输出设备通常是耳机和扬声器.

Android音频系统核心是Audio系统，负责音频方面的`数据传输`和`控制功能`，也负责`音频设备的管理`。Audio系统主要分成如下几个层次:

* Media库提供的Audio系统本地部分接口
* AudioFlinger作为Audio系统本地部分接口
* Audio的硬件抽象层提供底层支持。
* Audio接口通过JNI和Java框架提供给上层。

Audio音频系统基本层次结构：

<image src="image/01-01.png"/>
图中各个部分说明：

* Audio的Java部分：java部分代码路径`framework/base/media/java/android/media`与Audio先关的java包是`android.media`。其中包含了和`AudioManager`,`Audio`等系统相关的类。
* Audio的JNI部分：JNI部分的代码路径是:`framework/base/core/jni`.生成库是`libandroid_runtime.so`,Audio的JNI是其中一个部分。
* Audio的框架部分：框架部分的头文件是`framework/base/include/media`。源代码路径是`framework/base/media/libmeida`。Audio本地框架是Media库的一部分，本部分内容被编译成libmedia.so。提供Audio部分的接口(包括基于Binder的IPC机制)。

* AudioFlinger:Flinger部分的代码是`frameworks/base/libs/audioflinger`。此部分内容被编译为`libaudioflinger.so`。这是Audio系统的本地服务部分。
* Audio的硬件抽象层接口：硬件抽象层接口的头文件是`hardware/libhardware_legacy/include/hardware/`。Audio硬件抽象层实现各个系统中可能是不同的，需要使用代码去继承相应的类并实现它们。作为Android系统本地框架层和驱动程序接口。

## 2.分析音频系统层次

在Android中，Audio系统从上到下分别由Java的Audio类、Audio本地框架类、AudioFlinger和Audio的硬件抽象层几个部分。层次说明：

* Audio本地框架类：是libmedia.so的一部分，这些Audio接口对上层提供接口，由下层的本地代码去实现。
* AudioFlinger:继承了libmedia里面接口，提供实现库`libaudioflinger.so`，这部分内容没有自己对外的头文件，上层调用的只是`libmeida`本地接口，但实际调用的内容是`libaudioflinger.so`
* JNI:在Audio系统中，使用JNI和Java对上层提供接口，JNI部分通过调用libmedia库提供的接口来实现。
* Audio硬件抽象层:提供到硬件的接口，供AudioFlinger调用。Audio的硬件抽象层实际上是各个平台开发过程中需要关注和独立完成的部分。

因为Android中的Audio系统不涉及编解码环节，只负责上层系统和底层Audio硬件的交互，所以通常以PCM作为输入/输出格式。

在Android的Audio系统中，无论上层还是下层，都使用个管理类和输出/输入来表示整个Audio系统，输出/输入两个类负责数据通道。在各个层次之间具有对应关系：

|层次说明|Audo管理环节|Audio输出|Audio输入|
|--|--|--|--|
|Java层|android.media.AudioSystem|android.media.AudioTrack|android.media.AudioRecorder|
|本地框架层|AudioSystem|AudioTrack|AudioRecorder|
|AudioFlinger|IAudioFlinger|IAudioTrack|IAudioRecorder|
|硬件抽象层|AudioHardwareInterface|AudioStreamOut|AudioStreamIn|

### 2.1 Media库中的Audio框架
在Media库中提供了Android的Audio系统的核心框架，在库中实现了AudioSystem,AudioTrack和AudioRecorder这三个类。另外还提供了IAudioFlinger类接口，通过此类可以获得IAudioTrack和IAudioRecorder两个接口，分别用于声音的播放和录制。AudioTrack和AudioRecorder分别通过调用IAudioTrack和IAudioRecorder实现。Audio系统头文件保存在`frameworks/base/include/media`目录中，其中包含了如下头文件。

* `AudioSystem.h`:media库的Audio部分对上层的总管接口。
* `IAudioFlinger.h`:需要下层实现的总管接口。
* `AudioTrack.h`:放音部分对上接口。
* `IAudioTrack.h`:放音部分需要下层实现的接口。
* `AudioRecoder.h`：录音部分对上接口。
* `IAudioRecorder.h`：录音部分需要下层实现的接口。
其中文件`IAudioFlinger.h、IAudioTrack.h和IAudioRecoder.h`接口是通过下层的继承来实现的。文件`AudioFlinger.h、AudioTrack.h和AudioRecorder.h`是对上层提供的接口，它们既可以供本地调用（例如声音的播放器\录制器等），也可以用JNI向Java层提供接口。

从具体功能上看，AudioSystem用于综合管理Audio系统，而AudioTrack和AudioRecoder分别负责输出和输入音频数据，即分别实现播放和录制功能。

在文件AudioSystem.h中定义了枚举值和`set/get`等一系列接口，主要代码如下：

```cpp
class AudioSystem
{
public:
	enum stream_type{//Audio 流类型
		SYSTEM	=1,
		RING	=2,
		MUSIC	=3,
		ALARM	=4,
		NOTIFACTION	=5,
		BLUETOOTH_SCO=6,
		ENFORCED_AUDIBLE=7,
		NUM_STREAM_TYPES
	};
	enum audio_outpu_type{//Audio数据输出类型
		AUDIO_OUTPUT_DEFAULT =-1,
		AUDIO_OUTPUT_HARDWARE=0,
		AUDIO_OUTPUT_A2DP    =1,
		NUM_AUDIO_OUTPUT_TYPES
	};
	enum audio_format{//Audio数据格式
		FORMAT_DEFAULT =0,
		PCM_16_BIT,
		PCM_8_BIT,
		INVALID_FORMAT
	};
	enum audio_mode{ //Audio 模式
		MODE_INVALID =-2,
		MODE_CURRENT =-1,
		MODE_NORMAL  =0,
		MODE_RINGTONE,
		MODE_IN_CALL,
		NUM_MODES// not a valid entry,denotes end-of-list
	};
	enum audio_routes{ //Audio路径类型
		ROUTE_EARPIECE =(1<<0),
		ROUTE_SPEAKER  =(1<<1),
		ROUTE_BLUETOOTH_SCO=(1<<2),
		ROUTE_HEADSET  =(1<<3),
		ROUTE_BLUETOOTH_A2DP=(1<<4),
		ROUTE_ALL=-1UL,
	};
	enum audio_in_acoustics{
		AGC_ENABLE    =0x0001,
		AGC_DISABLE   =0,
		NS_ENABLE     =0X0002,
		TX_IIR_ENABLE =0x0004,
		TX_DISABLE    =0
	};
	static status_t speakderphone(bool state);
	static status_t isSpeakerphoneOn(bool* state);
	static status_t bluetoothSco(bool state);
	static status_t isBluetoothScoOn(bool *state);
	static status_t muteMicrophone(bool state);
	static status_t isMicphoneMuted(bool *state);
	static status_t setMasterVolume(float value);
	static status_t setMasterMute(bool mute);
	static status_t setMasterVolume(float*volume);
	static status_t getMasterVolume(float*volume);
	static status_t getMasterMode(int stream,bool mute);
	static status_t setStreamVolume(int stream,float value);
	static status_t setStreamMute(int stream,bool mute);
	static status_t getStreamVolume(int stream,float*volume);
	static status_t getStreamMute(int stream,float*volume);
	static status_t setMode(int mode);
	static status_t getMode(int*mode);
	static status_t setRouting(int mode,uint32_t routes,uint32_t mask);
	static status_t getRouting(int mode,uint 32_t routes,uint32_t mask);
	static status_t isMusicActive(bool*state);
	static status_t setParameter(const char*key,const char*value);
	static void setErrorCallback(audio_error_callback cb);
	static const sp<IAudioFlinger> & get_audio_flinger();
	static float linearToLog(int volume);
	static int logToLiner(float volume);
	static status_t getOutputSamplingRate(int*samplingRate,int steram=DEFAULT);
	static status_t getOutputFrameCount(int*frameCount,int stream=DEFAULT);
	static status_t getOutputLatency(uint32_t*latency,int stream=DEFAULT);
	static bool routedToA2spOutput(int streamType);
	static status_t getInputBuffer(uint32_t sampleRate,int format,int channelCount,size_t*buffSize);
}
```

audio_routes可以用“或”来表示。**AudioTrack**是Audio输出环节的类，其中包含最重要的接口`write()`。主要代码如下:

```cpp
class AudioTrack
{
	typedef void(*callback_t)(int event,void*user,void*info);
	AudioTrack(
		int streamType,
		uint32_t sampleRate =0,//音频采样率
		int format          =0,//音频格式8,16PCM
		int channelCount    =0,//音频通道数
		int frameCount      =0,//音频帧数
		uint32_t flags      =0,
		callback_t cbf      =0,
		void*user           =0,
		int notificationFrames=0
	);
	void start();
	void stop();
	void flush();
	void pause();
	void mute(bool);
	ssize_t write(const void*buffer,size_t size);
};
```
类AudioRecord用于实现和Audio输入相关的功能，其中重要的功能是通过接口函数`read()`实现的,实现如下：

```cpp
class AudioRecord
{
public:
	AudioRecord(int streamType,
			uint32_t sampleRate=0,//音频采样率
			int format         =0,//音频的格式
			int channlCount    =0,
			int frameCount     =0,
			uint32_t flags     =0,
			callback_t cbf     =0,
			void*user          =0,
			int notificationFrames=0);
	status_t  start();
	status_t  stop();
	ssize_t   read(void*buffer,size_t size);
};
```
在类AudioTrack和AudioRecord中，函数`read()`和`write()`参数都是内存的指针及其大小，内存中的内容一般表示Audio原始数据(PCM数据).这两个类还涉及Audio数据格式、通道数、帧数目等参数，不但可以建立时指定，还可以之后用`set()`函数进行设置。

另外，在`libmedia`库中提供的只是一个`Audio系统框架`。其中AudioSystem、AudioTrack和AudioRecord分别调用下层接口IAudioFlinger、IAudioTrack和IAudioRecord来实现。另外的一个接口是IAudioFlingerClient,作为向IAudioFlingerClient中注册的监听器，相当于使用回调函数获取IAudioFlinger运行时信息。

### 2.2本地代码
在Android系统中，AudioFlinger是Audio音频系统的中间层，能够为libmedia提供的Audio部分接口实现,`frameworks/base/libs/audioflinger`.文件AudioFlinger.h和AudioFlinger.cpp是实现AudioFlinger的核心文件，在其中提供了类AudioFlinger，此类是一个IAudioFlinger实现。

```cpp
class AudioFlinger:public BnAudioFlinger,public IBinder::DeathRecipient
{
public:
	static void instaniate();
	virtual status_t dump(int fd,const Vector<String16>& args);
	virtual sp<IAudioTrack> createTrack(
		pid_t pid,
		int streamType,
		uint32_t sampleRate,
		int format,
		int channelCount,
		int frameCount,
		uint32_t flags,
		const sp<IMemory>& sharedBuffer,
		status_t *status
	);
	virtual uint32_t sampleRate(int output) const;
	virtaul int     channelCount(int output) const;
	virtual int format(int output) const;
	virtual size_t frameCount(int output) const;
	virtual uint32_t latency(int output) const;
	virtual status_t setMasterVolume(float value);
	virtual status_t setMasterMute(bool muted);
	virtual  status_t setRouting(int mode,uint32_t routes,uint32_t mask);
	virtual  uint32_t getRouting(int mode) cosnt;
	virtual status_t setMode(int mode);
	virtual int      getMode() const;
	virtual sp<IAudioRecord> openRecord(
		pid_t pid,
		int streamType,
		uint32_t sampleRate,
		int format,
		int channelCount,
		int frameCount,
		uint32_t flags,
		status_t * status
	);
};
```
AudioFlinger使用函数createTrack()来创建音频输出设备IAudioTrack使用函数openRecord()来创建输入设备IAudioRecord，并且还使用接口get/set来实现控制功能。 构造函数AudioFlinger()代码'

```cpp
AudioFlinger::AudioFlinger()
{
	mHardwareStatus=AUDIO_HW_IDLE;
	mAudioHardware=AudioHardwareInterface::create();
	mHardwareStatus=AUDIO_HW_INIT;
	if(mAudioHardware->initCheck()==NO_ERROR){
		mHardwareStatus=AUDIO_HW_OUTPUT_OPEN;
		status_t status;
		AudioStreamOut* hwOutput=mAudioHardware->openOutputStream(AudioSystem::PCM_16_BIT,0,0,&status);
		mHardwareStatus=AUDIO_HW_IDLE;
		if(hwOutput){
			mHardwareMixerThread=new MixerThread(this,hwOutput,AudioSyste::AUDIO_OUTPUT_HARDWARE);
		}else{
			LOGE("falied to initalize hardware output stream ,status:%d",status);
		}
#ifdef WITH_A2DP
		mA2dpAudioInterface=new A2dpAudioInterface();
		AudioStreamOut*a2dpOutput=mA2dpAudioInterface->openOutputStream(AudioSystem::PCM_16_BIT,0,0,&status);
		if(a2dpOutput){
			mA2dpMixerThread=new MixerThread(this,a2dpOutput,AudioSyste::AUDIO_OUTPUT_A2DP);
			if(hwOutput){
				uint32_t frameCount=(a2spOutput->bufferSize()/a2dpOutput->frameSize()*hwOutput->sampleRate())/a2dpOutput->sampleRate();
				MixerThread::OutputTrack*a2dpOutTrack=new MixerThread::OutputTrack(mA2dpMixerThread,hwOutput->sampleRate(),
				AudioSystem::PCM_16_BIT,
				hwOutput->channelCount(),
				frameCount);
				mHardwareMixerThread->setOutputTrack(a2dpOutTrack);
			}
		}else{
			LOGE("failed to initalize A2DP output stream,status:%d",status);
		}
#endif
		setRouting(AudioSystem::MODE_NORMAL,AudioSyste::ROUTE_SPEAKER,AudioSysem::ROUTE_ALL);
		setRouting(AudioSystem::MODE_RINGTONE,AudioSyste::ROUTE_SPEAKER,AudioSystem::ROUTE_ALL);
		setRouting(AudioSystem::MODE_IN_CALL,AudioSyste::ROUTE_EARPIECE,AudioSyste::ROUTE_ALL);
		setMode(AudioSystem::MODE_NORMAL);
		setMasterVolume(1.0f);
		setMasterMute(false);
		mAudioRecordThread=new AudioRecordThread(mAudioHardware,this);
		if(mAudioRecordThread!=0){
			mAudioRecordThread->run("AudioRecordThread",PRIORITY_URGENT_AUDIO);
		}
	}else{
		LOGE("couldn;t even initialize the stubbed audio hardware!");
	}
}
```