# OpenCore 引擎
## 1.OpenCore 层次结构
OpenCore另一个常用称呼是PacketVideo（公司名字）,是Android多媒体核心。基于C++实现的，定义了全功能的操作系统移植层，各种基本功能都被封装成类的形式，各层次之间的接口多使用继承等方式。

OpenCore是一个多媒体的框架，从宏观上看主要如下两个方面：

* PVPlayer：提供了媒体播放器的功能，可以完成各种音频(Audio)、视频(Video)流的回放(Playback)功能。
* PVAuthor:提供媒体流记录的功能，可以完成各种音频(Audio)、视频(Video)流及静态图像捕获功能。

PVPlayer和PVAuthor以SDK的形式提供给开发者，可以这个SDK之上构建多种应用程序和服务。在移动终端中常常使用的多媒体应用程序有流媒体播放器、照相机、录像机、录音机等。

<image src="image/02-01.jpg"/>

* OSCL是Operating System Compatiblity Library的缩写，操作系统兼容库，其中包含了一些操作系统底层的操作，功能为了更好地在不同操作系统间移植。包含了基本数据类型、配置、字符串工具、I/O、错误处理、线程等内容，类似于一个基础C++库。
* PVME：PacketVideoMultimedia Framework缩写，PV多媒体框架，可以在框架内实现一个文件解析(parser)和组成(composer)、编/解码的NODE，也可以继承其通用的接口，用户实现一些NODE。
* PVPlayer Engine:PVPlayer引擎。
* PVAuthor Engine:PVAuthor引擎。

其实除了上面OpenCore中还包含了很多内容。

* 从播放角度看，PVPlayer输入的(Source)是文件或者网路媒体流，输出是(Sink)的是音频、视频的输出设备，其基本功能包含了媒体流控制、文件解析、音频、视频流的解码(Decode)等方面的内容。网络相关：RTSP流(Real time stream protocol)
* 媒体流记录方面，PVAuthor输入的(Source)是照相机、麦克风等设备，输出(Sink)的是各种文件，包含流的同步、音频、视频的编码(Encode),以及文件的写入等功能。

使用OpenCore SDK功能，有可能需要在应用程序实现一个适配器(Adapter)，然后再适配器之上实现具体的功能，对于PVMF的NODE也可以基于通用的接口，在上层用插件形式实现具体功能。

## 2.OpenCore代码结构
在Android系统中，OpenCore代码目录是`external/opencore`，此目录是OpenCore的根目录，其中包含各个子目录的具体说明如下:

* android:这是一个上层库，基于PVPlayer和PVAuthor的SDK实现了一个为Android使用的Player和Author。
* baselibs:其中包含了数据结构和线程安全等内容的底层库。
* codecs_v2:内容较多的库，包含编/解码的实现和OpenMAX的实现。
* engines:包含PVPlayer和PVAuthor引擎的实现。
* extern_libs_v2:包含了khronos的OpenMAX的头文件。
* fileformats:文件格式的解析(parser)工具。
* nodes:其中提供了PVMF的NODE，主要是编解码和文件解析方面的NODE。
* oscl:操作系统兼容库。
* pvmi:包含了输入/输出控制的抽象接口。
* protocols:主要包含了和网络相关的RTSP、RTP、HTTP等协议内容。
* pvcommon:pvcommon库文件的Android.mk文件，没有原文件。
* pvplayer:pvplayer库文件的Android.mk文件，没有源文件。
* pvauthor:pvauthor库文件的Android.mk没有源文件。
* tools_v2:其中包含了编译工具和一些可注册文件。
* Android.mk:全局的编译文件
* pvplayer.conf：配置文件。

在external/opencore的各个子文件还包含了很多个Android.mk文件，之间存在递归的关系例如根目录中:

```makefile
include $(PV_TOP)/pvcommon/Android.mk
include $(PV_TOP)/pvplayer/Android.mk
include $(PV_TOP)/pvauthor/Android.mk
```

## 3.OpenCore编译结构
在Android开源版本中，通过OpenCore编译出来各个库的具体说明：

* libopencoreauthor.so：OpenCore的Author库。
* libopencorecommon.so：OpenCore底层的公共库。
* libopencoredowloadreg.so:下载注册库。
* libopencoredownload.so:下载功能实现库。
* libopencoremp4reg.so:MP4注册库。
* libopencoremp4.so:MP4功能库
* libopencorenet_support.so:网络支持库
* libopencoreplayer.so:OpenCore的Player库。
* libopencorertspreg.so:RTSP注册库。
* libopencorertsp.so:RTSP功能实现库。

OpenCore中各个库之间的关系：

* libopencorecommon.so:所有库的依赖库，提供了公共的功能。
* libopencoreplayer.so:和libopencoreauthor.so：两个并立的库，分别用于回放和记录，而且这两个库是OpenCore对外的接口库。

