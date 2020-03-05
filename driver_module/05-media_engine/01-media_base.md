# 多媒体插件框架
将介绍OpenCore引擎和Stagefright引擎基本知识和移植方法。

## 1.Android 多媒体插件
在Android多媒体系统中，可以根据需要添加一些第三方插件，这样可以增强多媒体系统的功能。在Android系统的本地多媒体引擎上米昂，是Android的多媒体本地框架，而在多媒体本地框架上面是多媒体JNI和多媒体的Java框架部分，来提供多媒体API进行构建。

本章将介绍OpenCore引擎和Stagefright引擎，这些引擎是Android本地框架中定义接口的实现者，上层调用者不知道Android下层使用什么多媒体引擎。

<image src="image/01-01.jpg"/>

<image src="image/01-02.jpg"/>
从多媒体应用的实现角度来看，多媒体系统主要包含如下两方面：

* 输入输出环节：音频、视频纯数据流的输入、输出系统。
* 中间处理环节：包括文件格式处理和编码/解码环节处理。

假如想要处理一个MP3文件，媒体播放器的处理流程是：将一个MP3格式的文件作为播放器器的输入，将声音从播放器设备输出。在具体实现上，MP3播放器经过了MP3格式文件解析、MP3码流解码和PCM输出播放的过程。

## 2.需要移植的内容
移植多媒体插件时，主要包含两个方面：

* 输入/输出环节的工作：主要是基于Android硬件抽象层来实现的。
* 编码/解码环节：通常是基于OpenMax IL层实现的。

从Android2.2开始，OpenCore引擎和Stagefright引擎同时存在，并且主要通过Stagefright引擎实现媒体文件的播放。

OpenCore引擎和Stagefright引擎支持两种插件类型：一种是实现媒体输入/输出环节的插件，一种是实现编码/解码环节的插件。

* OpenCore引擎：使用MediaIO形式实现媒体播放器的实现输出功能，使用OpenMax编码和解码插件。
* Stagefright引擎：使用VideoRender形式实现媒体播放器的视频输出功能，使用Android封装的OpenMax接口事项编码和解码插件。
