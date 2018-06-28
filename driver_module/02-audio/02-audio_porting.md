# 移植Audio系统的必备技术
在Android中,Auido的标准化部分是硬件抽象层接口，所以针对不同的特定平台，需要移植`Audio驱动程序`和`Audio硬件抽象层`，开发人员任务是移植这两方面的内容.

## 1.移植Audio系统要做的工作

* Audio驱动程序：此部分需要在Linux内核中实现，虽然有很多实现方式，但是大多数Audio驱动程序都需要提供用于音量控制等功能的控制接口，通过这些接口实现PCM输入、输出的数据接口类。
* Audio硬件抽象层：这是Audio驱动程序和Audio本地框架类AudioFlinger的接口。根据Android系统对接口定义，Audio硬件抽象层是C++类的接口，需要在继承接口中定义三个类来实现Audio硬件抽象层，这三个类分别实现总控、输入和输出功能。

要实现一个Android的硬件抽象层，需要实现`AudioHardwareInterface`、`AudioStreamOut`和`AudioStreamIn`这三个类，并将代码编译成动态库`libaudio.so`。`AuidoFinger`会链接这个动态库,并调用其中的`createAudioHardware()`函数来获取接口。

在文件`AudioHardwareBase.h`中定义了类`AudioHardwareBase`,此类继承了`AudioHardwareInterface`,通过继承此接口也可以实现Audio的硬件抽象层。

Android系统的Audio硬件抽象层可以通过继承类`AudioHardwareInterface`类实现，其中分为控制部分和`输入、输出`处理部分。

## 2.分析硬件抽象层
Audio系统的硬件抽象层是AudioFlinger和Audio硬件之间的接口，不同系统的移植过程可以不同的实现方式，其中Audio硬件抽象层接口路径如下:`hardware/libhardware_legacy/include/hardware`.其中核心文件是`AudioHardwareBase.h`和`AudioHardInterface.h`。

作为Android系统的Audio硬件抽象层，既可以基于Linux标准的ALSA或OSS音频驱动实现，也可以基于私有的Audio驱动接口实现。

在文件AudioHardwareInterface.h中类分别定义了类AudioStreamOut、AudioStreamIn和AudioHardwareInterace。类AudioStreamOut和AudioStreamIn分别描述音频输出设备和音频输入设备。其中负责输出流的接口分别是函数`write()`和`read()`,参数是一块内存的指针和长度；另外还有一些设置和获取接口，类AudioStreamOut和AudioStreamIn的实现代码如下：

```cpp
class AudioStreamOut{
public:
	virtual ~AudioStreamOut()=0;
	virtual status_t setVolume(float volume)=0;
	virtual ssize_t write(const void*buffer,size_t bytes)=0;
	virtual int channelCount() const=0;
	virtual status_t dump(int fd,const Vector<String16>&args)=0;
};
class AudioStreamIn{
public:
	virtual ~AudioStreamIn()=0;
	virtual status_t setGain(float gain)=0;
	virtual ssize_t read(void*buffer,ssize_t bytes)=0;
	virtual int channelCount()const=0;
	virtual int format()const=0;
	virtual status_t setGain(float gain)=0;
	virtual status_t dump(int fd,const Vector<String16> &args)=0;
};
```
AudioStreamOut/In是两个接口类。两个类都需要通过Audio硬件抽象层的核心AudioHardWareInterface接口类来获取：

```cpp
class AudioHardwareInterface{
public:
		AudioHardwareInterface();
	virtual ~AudioHardwareInterface();
	virtaul status_t initCheck()=0;
	virtual status_t standby()=0;
	virtual status_t setVoiceVolume(float volume)=0;
	virtual  status_t setMasterVolume(float volume)=0;
	virtual status_t setRouting(int mode,uint32_t routes)=0;
	virtual status_t setRouting(int mode,uint32_t*routes);
	virtual status_t getRouting(int mode,uint32_t*routes);
	virtual status_t setMode(int mode)=0;
	virtual status_t getMode(int*mode)=0;
	virtual status_t setMicMute(bool*state)=0;
	virtual status_t getMicMute(bool*state)=0;
	virtual status_t setParameter(const char*key,const char*value);
	virtual AudioStreamOut* openOutputStream(
		int format=0,
		int channelCount=0,
		uint32_t sampleRate=0)=0;
	virtual AudioStreamIn*openStreamIn(
			int format,
			int channelCount=0,
			uint32_t sampleRate=0)=0;
	virtual status_t dumpState(int fd,const Vector<String16&>args);
	static AudioHardwareInterface* create();
};
```
除此之外，在文件AudioHardwareInterface.h中还定义了C语言的接口来获取一个AudioHardware Interface类型的指针:

```cpp
extern "C" AudioHardwareInterface* createAudioHardware(void);
```

## 3.分析AudioFlinger的Audio硬件抽象层的实现
在Android的AudioFlinger中，可以通过编译宏来选择Audio硬件抽象层，可选择的Audio硬件抽象层既可以作为参考设计，也可以在没有实际的Audio硬件抽象层时使用，目的是保证系统 正常运行。

### 3.1 编译文件
文件Android.mk是AudioFlinger的编译文件，定义如下：

```makefile
ifeq ($(strip $(BOARD_USES_GENERIC_AUDIO)),true)
	LOCAL_STATIC_LIBRARIES+=libaudiointercface
else
	LOCAL_SHARED_LIBRARIES+=libaudio
endif
LOCAL_MODULE:=libaudioflinger
include $(BULD_SHARED_LIBRARY)
```
在上述代码中，当BORAD_USES_GENERIC_AUDIO为true时链接`libaudiointerface.a`静态库；否则链接`libaudiointerface.so`动态库，大多数用后者。生成(`libaudiointerface.a`)如下:

```makefile
include $(CLEAR_VARS)
LOCAL_SRC_FILES:=\
	AudioHardwareGeneric.cpp \
	AudioHardwareStub.cpp \
	AudioDumpInterface.cpp \
	AuidoHardwareInterface.cpp \
LOCAL_SHARED_LIBRARIES:= \
	libcutils \
	libutils \
	libmedia \
	libhardware_legacy 
ifeq ($(strip $(BOARD_USES_GENERIC_AUDIO)),true)
	LOCAL_CFLAGS+= -DGENERIC_AUDIO
endif 
LOCL_MODULE:=libaudiointerface
include $(BUILD_STATIC_LIBRARY)
```
在上述代码中，4个源文件来生成libaudiointerface.a静态库。

* 1.`AudioHardwareInterface.cpp`：实现基础类和管理
* 2.`AudioHardwareGeneric.cpp`：实现基于特定驱动的通用Audio硬件抽象层。
* 3.`AudioHardwareStub.cpp`:实现Audio硬件抽象层的一个stub。
* 4.`AudioDumpInterface.cpp`:实现输出到文件的Audio硬件抽象层。

#### 3.1.1 基础和管理类
在文件`AudioHardwareInterface.cpp`，Audio硬件抽象层的创建函数：

```cpp
AudioHardwareInterface*AudioHardwareInterface::create()
{
	AudioHardwareInterface*hw=0;
	char value[PROPERTY_VALUE_MAX];
#ifdef GENERIC_AUDIO
	//调用Audio硬件抽象层
	hw=new AudioHardwareGeneric();
#else
	if(property_get("ro.kernel.qemu",value,0)){
		LOGD("Running in emulation - using generic audio driver");
		hw=new AudioHardwareGeneric();
	}else{
		LOGV("Creating Vendor Specific AudioHardware");
		//调用实际的Audio硬件抽象层
		hw=createAudioHardware();
	}
#endif
	if(hw->initCheck()!=NO_ERROR){
		LOGW("Using stubbed audio hardware.No sound will be produced");
		delete hw;
		//调用实际Audio Stub实现
		hw=new AudioHardwareStub();
	}
#ifdef DUMP_FINGER_OUT
	//此实际Audio的Dump接口实现
	hw=new AudioDumpInterface(hw);
#endif
	return hw;
}
```

#### 3.1.2 通用的Audio接口
在Andriod系统中，文件AudioHardwareGeneric.h和AudioHardwareGeneric.cpp实现了通用的Audio硬件抽象层，与Stub实现方式不同，这是一个真正能够使用的Audio抽象层，但是它需要Android的一种特殊的声音驱动程序的支持。

通用硬件层中，类`AudioStreamOutGeneric`、`AudioStreamInGeneric`和	`AudioHardwareGeneric`分别继承Audio硬件抽象层的三个接口。

```cpp
class AudioStreamOutGeneric:public AudioStreamOut{
};
class AudioStreamInGeneric:public AudioStreamIn{
};
class AudioHardwareGeneric:public AudioStreamBase{
};
```
在文件AudioHardwareGeneric.cpp中使用的驱动程序是`/dev/eac`，这是一个非标准程序，定义设备路径的代码：

`static char const*const kAudioDeviceName="/dev/eac";`

**注意**：eac驱动程序是一个misc程序，作为Android的通用音频驱动，写设备表示放音、读设备表示录音。

在Linux操作系统中，`/dev/eac/`驱动程序在文件系统中节点设备为10,次设备号自动生成。通过构造函数AudioHardwareGeneric()可以打开这个驱动程序的设备节点。

```cpp
AudioHardwareGeneric::AudioHardwareGeneric():mOutput(0),mInput(0),mFd(-1),mMicMute(false)
{
	mFd=::open(kAudioDevcieName,O_RDWR);
}
```

此音频设备时一个简单的驱动程序，在其中咩有很多设备接口，只是写设备表示放音，读设备表示录音。放音和录音支持的都是16PCM：

```cpp
ssize_t AudioStreamOutGeneric::write(const void*buffer,size_t bytes)
{
	Mutex::Autolock _l(mLock);
	return ssize_t(::write(mFd,buffer,bytes);
}
ssize_t AudioStreamInGenderic::read(void*buffer,ssize_t bytes)
{
	AudioMutex lock(mLock);
	if(mFd<0){
		return NO_INIT;
	}
	return ::read(mFd,buffer,bytes);
}
```
尽管AudioHardwareGeneric是一个可以真正工作的Audio硬件抽象层，到那时这种实现方式非常简单，不支持各种设置，参数也只能使用默认的。而且这种驱动程序需要在Linux核心加入eac驱动程序的支持。
#### 3.1.3 AudioStub实现
在文件AudioHardwareStub.h和AudioHardwareStub.cpp中，通过Stub实现Android硬件抽象层。Stub方式不操作实际的硬件和文件，只是进行空操作，在系统没有实际的Audio设备时才使用Stub方式实现，目的是保证系统的正常运行。

如果使用这个硬件抽象层，实际上Audio系统的输入和输出都将为空。在文件AudioHardwareStub.h中定义了类AudioStreamOutStub和AudioStreamInStub。分别实现输入和输出功能。

```cpp
class AudioStreamOutStub:public AudioStreamOut{
public:
	virtual status_t set(int format,int channelCount,uint32_t sampleRate);
	virtual uint32_t sampleRate()const{return 44100;}
	virtual size_t bufferSize()const{return 4096}
	virtual int channelCount()const{return 2;}	
	virtual int format()cosnt {return AudioSystem::PCM_16_BIT;}
	virtual uint32_t latency()const{return 0;}
	virtual status_t setVolume(float volume){return NO_ERROR;}
	virtual ssize_t write(const void*buffer,size_t bytes);
	virtual status_t standby();
	virtual status_t dump(int fd,cosnt Vector<String16>&args);
};

class AudioStreamInStub:public AudioStreamIn{
public:
	virtual status_t set(int format,int channlCount,uint32_t sampleRate,AudioSystem::audio_in_acoustics acoustics);
	virtual uint32_t sampleRate()const{return 8000;}
	virtual size_t bufferSize()const{return 320;}
	virtual int channelCount()const{return 1;}
	virtual int format()const{return AudioSystem::PCM_16_BIT;}
	virtual status_t setGain(){return NO_ERROR};
	virtual ssize_t read(void*buffer,ssize_t bytes);
	virtual status_t dump(int fd,const Vector<String16>&args);
	vitual status_t standby(){return NO_ERROR;}
};
```
上述代码中，只用缓冲区大小、采样率和通道数，这个三个固定的参数将一些函数直接无错误返回。然后通过使用AudioHardwareStub类来继承AudioHardwareBase,也就是继承AudioHardwareInterface.

```cpp
class AudioHardwareStub:public AudioHardwareBase
{
public:
		AudioHardwareStub();
	virutal ~AudioHardwareStub();
	virtual status_t initCheck();
	virtual status_t setVoiceVolume(float volume);
	virtual status_t setMasterVolume(float volume);
	virtual status_t  setMicMute(bool state){mMicMute=state;return NO_ERROR;}
	virtual status_t getMicMute(bool*state)
	{*state=mMicMute;return NO_ERROR;}
	virtual status_t setParameter(const char*key,const char*value)
	{return NO_ERROR;}
	virtual AudioStreamOut*openOutputStream(
	int format=0,
	int channelCount=0,
	uint32_t sampleRate=0,
	status_t*statue=0);
	virtual AudioStreamIn*openInputStream(
		int format,
		int channelCount,
		uint32_t sampleRate,
		status_t* status,
		AudioSystem::audio_in_acousitcs acoustics);
};
```
为了保证可以输入和输出声音，Stub实现的主要内容是实现AudioStreamOutStub和AudioStreamInStub类的写/读函数：

```cpp
ssize_t AudioStreamOutStub::write(const void*buffer,size_t bytes)
{
	usleep(bytes*1000000/sizeof(int16_t)/channelCount()/sampleRate());
	return bytes;
}
ssize_t AudioStreamInStub::read(void*buffer,ssize_t bytes)
{
	usleep(bytes*1000000/sizeof(int16_t)/channelCount()/sampleRate();
	memset(buffer,0,bytes);
	return bytes;
}
```
当使用这个接口来输入和输出音频时，真实的设备并没有任何关系，输出和输入都是使用延时来完成，在输出时不会播放声音，但是返回值表示全部内容已经输出完成：在输入时会返回全部为0的数据。

### 3.1.4 具备Dump功能的Audio硬件抽象层
在文件`AudioDumpInterface.h`和`AudioDumpInterface.cpp`中提供了Dump功能的Audio硬件抽象层，目的是将输出的Audio数据写入到文件中。

其实AudioDumpInterface本身支持Audio输出功能，但是不支持输入功能。在文件AudioDumpInterface.h中定义：

```cpp
class AudioStreamOuDump:public AudioStreamOut
{
public:
		AudioStreamOutDump(AudioStreamOut*FinalStream);
		~AudioStreamOutDump();
	virtual ssize_t write(const void*buffer,size_t bytes);
	virtual uint32_t sampleRate()const{return mFinalStream->sampleRate();}
	virtual size_t bufferSize()const
	{return mFinalStream->bufferSize();}
	virtual int channelCount()const
	{return mFinalStream->channelCount();}
	virtual int format()const
	{return mFinalStream->format();}
	virtual uint32_t latency()const
	{return mFinalStream->latency();}
	virtual status_t setVolume(float volume)
	{return mFinalStream->setVolume(volume);}
	virtual status_t standby();
};
class AudioDumpInterface:public AudioHardwareBase
{
	virtual AudioStreamOut*openOutStream(
		int format=0,
		int channelCount=0,
		uint32_t sampleRate=0,
		status_t *status=0
	);
};
```
在上述代码中，只实现了AudioStreamOut输出，而没有实现AudioStreamIn输入。由此可见，此Audio硬件抽象层只支持输出功能，不支持输入功能。
`#define FINGER_DUMP_NAME "/data/FingerOut.pcm"`
在文件AudioDumpInterface.cpp中，通过函数AudioStreamOut实现写操作，写入的对象就是这个文件:

```cpp
ssize_t AudioStreamOutDump::write(const void*buffer,size_t bytes)
{
	ssize_t ret;
	ret=mFinalStream->write(buffer,bytes);
	if(!mOutFile&&gFirst){
		gFirst=false;
		mOutFile=fopen(FINGER_DUMP_NAME,"r");
		if(mOutFile){
			//打开输出文件
			mOutFile=fopen(FINGER_DUMP_NAME,"ab");
		}
	}
	if(mOutFile){
		//写文件输出内容
		fwrite(buffer,bytes,1,mOutFile);
	}
	return ret;
}
```

如果文件是打开的，则可以使用追加方式写入。所以当时用这个Audio硬件抽象层时，播放的内容(PCM)将全部被写入文件，而且这个类支持各种格式的输出，这取决于调用者的设置。

使用AudioDumpInterface的目的并不是为了实际的应用，而是**为了调试我们使用的类**。当使用播放器调试音频时，有时无法确认是解码器的问题还是Audio输出单元问题，这时就可以用这个类类代替实际的Audio硬件抽象层，`将解码器输出的Audio的PCM数据写入文件中，因此可以判断解码器的输出是否正确。`

### 4.真正实现Audio硬件抽象层
想要实现一个正真的Audio硬件抽象层，需要完成和2节中实现硬件抽象层类似的工作。例如可以基于Linux标准的音频驱动OSS(Open Sound System)或ALSA(Advanced Linux Sound Architecture)驱动程序来实现。

* 1.`基于OSS驱动程序实现`：实现和AudioHardwareGeneric方式类似。数据流的读/写操作通过对`/dev/dsp`设备的"读/写"来完成。区别在于OSS支持了更多的ioctl来进行设置，还涉及通过`/dev/mixer`设备进行控制，并支持更多不同的参数。
* 2.`基于ALSA驱动程序实现`：对于ALSA驱动程序来说，实现方式一般不是直接调用驱动程序的设备节点，而是先实现用户空间`alsa-lib`，然后Audio硬件抽象层通过调用`alsa-lib`来实现。

在实现Audio硬件抽象层时，如果系统中有多个Audio设备，此时可由硬件抽象层自行处理`setRouting()`函数设定。例如可以选择多个支持设备的同时输出，或者有优先级输出。对于这种情况，数据流一般来自函数`AudioStreamOut::write()`.可由硬件抽象层确定输出方法。对于某种特殊情况，也有可能采用硬件直接连接的方式，此时数据流可能并不是来自上面`write()`，这样就咩有数据通道，只有控制接口。Audio硬件抽象层也是可以处理这种情况的。