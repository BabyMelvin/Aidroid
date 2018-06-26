# 视频输出系统驱动
本章将详解Android视频输出系统驱动实现和移植内容。

## 1.视频输出系统结构
在Android系统中，视频输出系统对应的是`Overlay子系统`，此系统是Android的一个可选系统，用于**加速显示输出视频数据**。视频输出系统的硬件通常叠加在主显示区之上的额外的叠加显示区。**这个额外的叠加显示区和主显示区使用独立的显示内存**。

* `主显示区`用于输出图形系统，通常是RGB颜色空间。
* `额外显示区`用于输出视频，通常是YUV颜色空间。

主显示区和叠加显示区通过Blending(硬件混合)自动显示在屏幕上。在软件部分我们无须关心叠加的实现过程，但是可以控制叠加的层次顺序和叠加层的大小等内容。

<image src="image/01-01.png"/>

Android中的Overlay系统没有Java部分，在其中只包含了视频输出的驱动程序、硬件抽象层和本地框架等。Overlay系统的结构：

<image src="image/01-02.png"/>
在系统结构中，各个组成：

* Overlay驱动程序：通常是基于FrameBuffer或V4L2的驱动程序。在此文件中主要定义了两个struct，分别是`data device`和`control device`.这两个结构体分别针对`data device`和`control deivce`的函数`open()`和函数`close()`.这两个函数是注册到`device_module`里面的函数。
* Overlay硬件抽象层：`hardware/libhardware/include/hardware/overlay.h`Overlay硬件抽象层是Android中一个标准的硬件模块，其接口只有一个头文件。
* Overlay服务部分：`framework/base/libs/surfaceflinger/`。Overlay系统的服务部分包含在`SurfaceFlinger`中，此层次的内容比较简单，主要功能是通过类LayerBuffer实现的。首先明确的是SurfaceFlinger只是负责控制`merge Surface`,例如：计算两个`Surface`重叠的区域。至于Surfaece需要显示的内容，则通过`Skia`、`Opengl`和`Pixflinger`来计算。在介绍SurfaceFlinger之前先忽略里面存储的内容究竟是什么，先弄清楚它对`merge`的一系列控制的过程，然后再结合2D、3D引擎来看它处理过程。
* 本地框架代码。本地框架头文件`framework/base/include/ui`。

Overlay系统只是整个框架的一部分，主要通过类loverlay和Overlay实现的，源代码被编译成libui.so，它提供的API主要在视频输出和照相机取景模块中使用。

## 2.需要移植的部分
因为Overlay系统底层和系统框架接口是硬件抽象层，所以想要实现Overlay系统，需要实现硬件抽象层和下面的驱动程序。在Overlay系统的硬件抽象层中，使用了Android标准硬件模块的接口，此接口是标准C语言接口，通过函数和指针来实现具体功能。其中报验了数据流接口和控制接口，需要根据硬件平台的具体情况来实现。

Overlay系统驱动程序通常是视频输出驱动程序，可以通过标准的FrameBuffer驱动程序或Video for linux2视频输出驱动方式来实现。因为系统不同，所以即使使用同一种驱动程序，也有不同的实现方式。

* 1.`FrameBuffer`驱动程序方式：FrameBuffer驱动程序方式是最直接的方式，实现视频输出从驱动程序的角度和一般FrameBuffer驱动程序类似。区别是视频输出使用YUV格式颜色空间，而用于图形界面的FrameBuffer使用RGB颜色和空间。
* 2.`Video for Linux2`方式.是Linux视频系统的一个标准框架，在其第一个版本中提供了摄像头视频输入框架，从2.0开始提供视频输出接口，使用此视频输出接口，可以根据系统的性能来调整队列的数目。