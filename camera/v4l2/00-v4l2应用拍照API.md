# V4l2 拍照API 调用详解
v4l2 操作实际上就是 `open()`设备， `close()`设备，以及中间过程的`ioctl()`操作。对于 ioctl 的调用，要注意对 errno 的判断，如果调用被其他信号中断，即 errno 等于 EINTR 的时候，要重新调用。

Video capture device 的实际功能就是采集视频信号，并将数字化的图像保存在 memory 中，现在几乎上所有的相关设备都能采集 25/30 帧 /s 。在下面的讨论中，我只列举出一些和 camera 密切相关的一些属性和方法。

# 1. open_device
打开设备一般都是使用`open()`打开 /dev 下的 video 设备文件 ，比如说 `/dev/video1`, 打开之前首先要对相应的设备文件进行检查 ，比如说使用`stat()`获得文件属性，并判断是否为字符设备文件。

驱动通过主设备号 81 和 0 ～ 255 之间的次设备号来注册 device note ，系统 管理 员通过设备的主次设备号在 `/dev` 目录下创建相应的字符设备文件。应用 程序不能通过设备的主次设备号来打开设备，而**必须通过适当的 device name** ，即 `/dev` 目录下的设备文件来打开设备。

v4l2 支持一个设备文件可以被多次打开，却只允许其中一个应用程序与设备进行数据 交换 ，其他应用程序只能用来设定一些设备参数，对设备进行一些控制.[解释](http://v4l2spec.bytesex.org/spec/c174.htm#OPEN)

# 2.init_device
对设备进行初始化是一个很复杂的过程，其中要进行一系列参数的协商，其中重要的包括 v4l2_capability, v4l2_cropcap, v4l2_format 等等。

首先使用 VIDIOC_QUERYCAP 命令 来获得当前设备的各个属性，查看设备对各项功能的支持程度：

`int ioctl(int fd, int request, struct v4l2_capability *argp);`

所有的 v4l2 驱动都必须支持 VIDIOC_QUERYCAP ，而且在打开设备以后，这个 ioctl 必须是被**首先调用**的。

v4l2_capability 的各项参数可以查 API ，其中比较重要的是下面的成员变量：

* _u32 capabilities:这个 32 位无符号整型定义了当前设备对一些关键属性的支持
	* V4L2_CAP_VIDEO_CAPTURE 0x00000001:这个设备支持 video capture 的接口，即这个设备具备 video capture 的功能
	* V4L2_CAP_VIDEO_OUTPUT 0x00000002:这个设备支持 video output 的接口，即这个设备具备 `video output` 的功能
	* V4L2_CAP_VIDEO_OVERLAY 0x00000004: 这个设备支持 video overlay 的接口，即这个设备具备 video overlay 的功能，在这个功能下会将采集到的 imag 方在视频设备的 meomory 中保存，并直接在屏幕上显示，而**不需要经过其他的处理**。
	* V4L2_CAP_VIDEO_OUTPUT_OVERLAY 0x00000200:这个设备支持 video output overlay( 又名 On-Screen Display) ，这是一个实验性的功能， spec 说明他在将来可能会改变，如果打开这个功能必须将 video overlay 功能给关闭，反之亦然
	* V4L2_CAP_READWRITE 0x01000000:这个设备是否支持`read()`和`write() I/O`操作函数
	* V4L2_CAP_STREAMING 0x04000000: 这个设备是否支持 `streaming I/O` 操作函数

在实际操作过程中，可以将取得的 capabilites 与这些宏进行与运算来判断设备是否支持相应的功能。除了 VIDIOC_QUERYCAP 之外，设备其他属性的获得可以通过其他的命令，比如说 VIDIOC_ENUMINPUT 和 VIDIOC_ENUMOUTPUT 可以枚举出设备的输入输出物理连接。[相关](http://v4l2spec.bytesex.org/spec/x282.htm)

获得 device 的 capability 以后，可以根据应用程序的功能要求对设备参数进行一系列的设置 ，这些参数又分为两部分

* 1.一个是 user contrl
* 2.一个是 extended control

下面先来讲对 user contrl 的一些参数进行设置。

User control 参数包含一个 ID ，以及相应的 Type ，下面对各个 type 进行简单的列举：

* ID Type
	* V4L2_CID_BASE:第一个预定义的 ID ，实际等于 V4L2_CID_BRIGHTNESS ，因为 V4L2_CID_BRIGHTNESS 是第一个预定义的 ID
	* V4L2_CID_USER_BASE:实际上等同于 V4L2_CID_BASE
	* V4L2_CID_BRIGHTNESS integer:图片的亮度，或者说黑色位准
	* V4L2_CID_AUTO_WHITE_BALANCE boolean:camera 的自动白平衡
	* V4L2_CID_EXPOSURE integer:camera 的爆光时间
	* V4L2_CID_LASTP1:最后一个预定义的 ID ，实际等于上一个 ID ＋ 1
	* V4L2_CID_PRIVATE_BASE:第一个 driver 定义的一般 control ID

可以通过 VIDIOC_QUERYCTRL 和 VIDIOC_QUERYMENU ioctls 来枚举出有效的**control ID** ，及其**属性**

比如说 ID 值，类型，是否有效，是否可修改，最大值，最小值，步长等等 ，主要的数据结构是 v4l2_queryctrl 和 v4l2_querymenu ，他们的结构可以参考 spec 。另外可以通过 V4L2_CID_BASE 和 V4L2_CID_LASTP1 可以枚举出所有的预定义 control ID ，可以通过 V4L2_CID_PRIVATE_BASE 来枚举出所有的驱动定义的 control ID 。 Menu 实际上是**同一个ID可能具有多个选项的目录**。

```c
int ioctl(int fd, int request,struct v4l2_queryctrl *argp);
int ioctl(int fd, int request, struct v4l2_querymenu *argp);
```
获得 user control ID 以后，可以对其中可以修改的 ID 按照应用程序的要求**进行修改** `VIDIOC_G_CTRL`, `VIDIOC_S_CTRL `

`int ioctl(int fd, int request, struct v4l2_control *argp);`v4l2_control 的结构比较简单，就是相应的 ID 及其 value 。[参考](http://v4l2spec.bytesex.org/spec/x542.htm)

除了 user control 之外还有一个就是扩展控制，扩展控制可以同时原子的对多个 ID 进行 control ，相关命令是三个： VIDIOC_G_EXT_CTRLS, VIDIOC_S_EXT_CTRLS 和 VIDIOC_TRY_EXT_CTRLS ：

`int ioctl(int fd, int request, struct v4l2_ext_controls *argp);`其中最重要的是 v4l2_ext_controls 这个数据结构，它包含几个内容：

* `__u32 ctrl_class`:现在 spec 中只定义了两种类型的 class ： `V4L2_CTRL_CLASS_USER` 和 `V4L2_CTRL_CLASS_MPEG`
* `__u32 count`:ctrl 数组中的 control ，即 v4l2_ext_control 的个数
* `struct v4l2_ext_control * controls`:control数组， v4l2_ext_control包含要设定的 ID ，以及value应用程序可以使用 V4L2_CTRL_FLAG_NEXT_CTRL 来对扩展 control 进行枚举，V4L2_CTRL_FLAG_NEXT_CTRL 返回下一个 ID 更高的 control ID ：

```c
struct v4l2_queryctrl qctrl;
qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
while (0 == ioctrl(fd, VIDIOC_QUERYCTRL, &qctrl)) {
	/* ... */
	qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
}

//要枚举指定的 control class 中的 control 可以使用下面的方法：

qctrl.id = V4L2_CTRL_CLASS_MPEG|V4L2_CTRL_FLAG_NEXT_CTRL;
while (0 == ioctl(fd, VIDOC_QUERYCTRL, &qctrl)) {
	if(V4L2_CTRL_ID2CLASS(qctrl.id) != V4L2_CTRL_CLASS_MPEG)
		break;
	
	/* ... */
	qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
}
```

`====================== camera control==========================`
当然前提是驱动必须提供对 V4L2_CTRL_FLAG_NEXT_CTRL 的支持。应用程序可以为创建一个控制面板，其中包含一系列控制，每个 control class 用一个 V4L2_CTRL_TYPE_CTRL_CLASS 类型开始，当使用 VIDIOC_QUERYCTRL 的时候将返回这个 control class 的 name ，下面我们来看`camera control class`的一些 control ：

* ID Type
	* V4L2_CID_CAMERA_CLASS class:camera class 的描述符，当调用 VIDIOC_QUERYCTRL 的时候将返回一个对这个 class 的描述
	* V4L2_CID_EXPOSURE_AUTO integer:自动爆光
	* V4L2_CID_FOCUS_AUTO boolean:自动对焦

[参考](http://v4l2spec.bytesex.org/spec/x802.htm)

对各种控制参数进行设置以后，下面要进行的就是要获得设备对 Image Cropping 和 Scaling 的支持，即对图像的取景范围以及图片的比例缩放的支持。 Image Crop 的功能通俗一点讲就是他对 camera 镜头能捕捉的图像，截取一个范围来保存，而这个要截取的范围就是最终保存下来的图像。

对于一个视频捕捉或者视频直接播放的设备来说，源是视频信号，而 cropping ioctl 决定视频信号的哪部分被采样，目标则是应用程序或者屏幕上读到的图片。对于视频输出设备来说，输入是应用程序传入的图片，而输出则是视频流， cropping ioctl 此时则决定图片的哪部分会被插入视频信号，所有的视频捕捉和视频输出设备都必须支持 VIDIOC_CROPCAP ioctl:

`int ioctl(int fd, int request, struct v4l2_cropcap *argp)`

其中数据结构 v4l2_cropcap 的几个要重要的成员变量是下面这些：

* `enum v4l2_buf_type type`:数据流的类型，在 VIDIOC_CROPCAP 这个控制中只有 V4L2_BUF_TYPE_CAPTURE, V4L2_BUF_TYPE_OUTPUT, V4L2_BUF_TYPE_OVERLAY 以及驱动定义的一些一般类型 V4L2_BUF_TYPE_PRIVATE 是有用的
* `struct v4l2_rect bounds`:这是 camera 的镜头能捕捉到的窗口大小的局限，在应用程序设置窗口参数的时候要注意，不能超过这个长宽限制
* `struct v4l2_rect defrect`: 定义了默认的窗口大小，包括起点的位置以及长宽的大小，大小以像素为单位
* `struct v4l2_fract pixelaspect`:定义了图片的宽高比

应用程序可以使用 VIDIOC_G_CROP 和 VIDIOC_S_CROP 来获得对这些窗口参数并对其进行设置，也就是所谓的 Scaling Adjustments ，因为硬件可能在这些窗口参数设置上具有很多限制，当需要对窗口参数进行设置的时候，驱动会按照自身的规律在应用程序要求和设备限制上决定一个平衡值，一 般应用程序应该先使用 VIDIOC_CROPCAP 来获得硬件限制，并使设定的参数在 bound 范围以内：

```c
int ioctl(int fd, int request, struct v4l2_crop *argp);
int ioctl(int fd, int request, const struct v4l2_crop *argp);
```

[参照](http://v4l2spec.bytesex.org/spec/x1904.htm)

设置好`取景窗口参数`以后，下面要进行的设置就是对**图形格式**的协商，这个 Data Format 的协商通过 VIDIOC_G_FMT 和 VIDIOC_S_FMT 来实现。另外 VIDIOC_TRY_FMT 的功能等同与 VIDIOC_S_FMT ，唯一的不同就是他不会改变驱动的状态，它在任何时候都可以被调用，主要用来获得硬件的限制，从而对参数进行协商。如果驱动需要与应用程序交换数据，则必 须支持 VIDIOC_G_FMT 和 VIDIOC_S_FMT ， VIDIOC_TRY_FMT 是可选的，但是是强烈推荐实现的。

```c
int ioctl(int fd, int requeset, struct v4l2_format *argp) ;
```

前面讲过，虽然一个设备文件可以支持多打开，但是只允许一个能与驱动进行数据交换，因此在设备的初始化过程中对 **VIDIOC_S_FMT ioctl 的调用是一个转折点**，第一个调用 VIDIOC_S_FMT ioctl 的文件描述符会打开一个**逻辑的流** ，如果此时其他的文件描述符对设备进行的操作有可能破坏这个流的时候是会被禁止的，比如说如果另外一个应用程序想修改 video standard ，只有**对流拥有所有权的文件描述符**才能修改这方面的属性。再比如当 overlay 已经开始的时候， video capture 就会被限制在和 overlay 相同的 cropping 和 image size 。

一般来说只允许同一个文件描述符拥有一个逻辑流，唯一的例外是 video capture 和 video overlay 可以使用同一个文件描述符。

下面来看看 v4l2_format 这个数据结构，它包含几个重要内容：

* `enum v4l2_buf_type type`:buf 的类型，比如说 V4L2_BUF_TYPE_VIDEO_CAPTURE
* `union fmt`
* `struct v4l2_pix_format`:used for video capture and output
* `struct v4l2_window`:used for video overlay

其中最重要的是 union 中的两个结构体， v4l2_window 是 overlay interface 的内容，将在 overlay 中再讨论，先看一下 v4l2_pix_format 的结构:

* `__u32 width`,`__u32 height`: 分别是 image 的宽度和高度，以像素为单位，应用程序可以设置这些参数，驱动会返回一个最靠近这些参数的值，为什么是最靠近的值呢，因为图像格式以及硬件限制的原因，可 能应用程序要求的值无法得到满足。**科普**:一个基础 知识 ， YUV 格式有两种存储方式，一种就是将其 3 个分量存在同一个数组中，然后几个像素组成一个宏块，这种方式叫 packed ；另外一种就是 3 个分量分别存放在不同的数组中，这种方式叫做 planar 
* `__u32 pixelformat`: 这就是图像格式了，可以是 RGB ，也可以是 YUV ，还可以是压缩格式 MPEG 或者 JPEG ，这个值是通过一个 4 字母宏来计算出来的：`#define v4l2_fourcc(a,b,c,d)(((__u32)(a)<<0) | ((__u32)(b)<<8)| ((__u32)(c)<<16) | ((__u32)(a)<<24)) `具体格式的标准宏可以参照 spec 。
* `enum v4l2_field field`:这个定义了视频信号的场的顺序，比如视频信号可能是顺序扫描的，也可能是隔行扫描的。分为top 场和 bottom 场，一个 video camera 不会在一个时间内暴光一个整帧，而是将其分成场分别传输。所有的 video capture 和 output 装置都必须指定其场的传输顺序，即是 top 场在前还是 bottom 场在时间上和空间 上的顺序 。具体的可以从参考 spec 关于 Field Order 的描述，一般采用的是 V4L2_FIILED_INTERLACED ，在这个模式下 image 包含交叉存取的帧，场的顺序由当前的视频标准来决定。
* `__u32 bytesperline`:即每行像素所占的 byte 数，应用程序和驱动都可以设置这个参数，但驱动可以忽略应用程序的参数，而返回一个硬件要求的参数，应用程序可以设置这个参数为 0 来让驱动返回一个默认值。 Image 在内存中还是按照每行像素这样来存储的，每一行像素后面都有一个衬垫来代表该行像素的结束。
* `__u32 sizeimage`:要保存一个完整的 Image 需要的 buffer 空间，单位是 byte ，由驱动来设定，是保存一个图像所需要的最大 byte 数，而不是图像被压缩的 byte 数。如果驱动需要与应用程序交换 image data 则必须支持 VIDIOC_ENUM_FMT 来列出所有驱动支持的 FMT 格式 ：［实际上， crop 是对取景进行限制，而 fmt 则是对**最终保存下来的图片属性**进行设置 ，如果取景后的图片和要求的图像属性有冲突，就要将**取景后的图片**进行相应的调整，比如放大，缩小等等 ］

看下面的例子：

```c
//Resetting the cropping parameters(A video capture device is assumed; change V4L2_BUF_TYPE_VIDEO_CAPTURE for other devices.)
struct v4l2_cropcap cropcap;
struct v4l2_crop crop;
memset(&cropcap, 0, sizeof(cropcap));

cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
if(-1 == ioctl(fd, VIDIOC_CROPCAP, &cropcap) {
	perror("VIDIOC_CROPCAP");
	exit(EXIT_FAILURE);
}

memset(&crop, 0, sizeof(crop));
crop.c = cropcap.defrect;

/* Ignore if cropping is not supported (EINVAL). */
if (-1 == ioctl(fd, VIDIOC_S_CROP, &crop) && errno != EINVAL) {
	perror("VIDOC_S_CROP");
	exit(EXIT_FAILURE);
}

// sample downscalling(A video capture device is assumed)
struct v4l2_cropcap cropcap;
struct v4l2_format format;
reset_cropping_parameters ();
/* Scale down to 1/4 size of full picture. */
memset (&format, 0, sizeof (format)); /* defaults */
format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
format.fmt.pix.width = cropcap.defrect.width >> 1;
format.fmt.pix.height = cropcap.defrect.height >> 1;
format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
if (-1 == ioctl (fd, VIDIOC_S_FMT, &format)) {
	perror ("VIDIOC_S_FORMAT");
	exit (EXIT_FAILURE);
}
/* We could check the actual image size now, the actual scaling factor
or if the driver can scale at all. */
```
另外还有一个可选的选项，就是如果采用`read/write`模式，还可以通过设置流参数属性来优化 capture 的性能，在这里就不讨论了，具体的可以去参照 spec 。

完成这一系列参数的初始化以后，最后一个要协商的就是 `I/0` 模式的选择：主要分为两种

* 一种是`Read/Write`，这也是打开 video device 之后默认选择的 I/O 方法，其他的方法如果使用必须经过协商；
* 还有一个就是stream ，其中stream中根据实现方式的不同又可以分为`Memory Mapping`和`User Pointers`。Driver 可以决定是否支持对`I/O`的**switch**，这不是必须的，如果 driver 不支持，则只有通过`open/close`device 来实现`I/0`的切换。

**具体两种方式**：

* 首先来看`Read/Write`，如果 VIDIOC_QUERYCAP 调用返回的 v4l2_capability 参数中， V4L2_CAP_READWRITE 被设置成真了的话，就说明支持` Read/Write I/O`。这是最简单最原始的方法，它**需要进行数据的拷贝** ( 而不是像 memory map 那样只需要进行指针的交换 ) ，而且不会交换**元数据** ( 比如说帧计数器和时间戳之类的可用于识别帧丢失和进行帧同步 ) ，虽然它是最原始的方法，但因为其简单，所以对于简单的应用 程序比如只需要**capture 静态图像**是很有用的 。如果使用 Read/Write 方法支持的话，必须同时支持另外两个函数`select()`和`poll()`，这两个函数用来进行`I/0`的多路复用。
* 对于 streaming 它有两种方式， driver 对两种方式的支持要使用`VIDIOC_REQBUFS`来确定:`int ioctl(int fd, int request, struct v4l2_requestbuffers *argp);`
	* 对于 memory mapped 方式， Memory mapped buffers 是通过 VIDIOC_REQBUFS 在 device memory中申请的，而且必须在map进应用程序虚拟地址空间 之前就申请好。
	* 而对于 User pointers ， User buffers 是在应用程序自己开辟的，只是通过 VIDIOC_REQBUFS 将驱动转化到 user pointer 的 I/O 模式下。这两种方式都不会拷贝数据，而只是 buffer 指针的交互。


下面我们来仔细看看`v4l2_buffer`这个数据结构：

* `__u32 index`:应用程序来设定，仅仅用来申明是哪个 buffer
* `enum v4l2_buf_type type`
* `__u32 bytesused`:buffer 中已经使用的 byte 数，如果是 input stream 由 driver 来设定，相反则由应用程序来设定
* `__u32 flags`:定义了 buffer 的一些标志位，来表明这个 buffer 处在**哪个队列**，比如输入队列或者输出队列 (`V4L2_BUF_FLAG_QUEUED` `V4L2_BUF_FLAG_DONE`) ，是否关键帧等等，具体可以参照 spec
* `enum v4l2_memory memory`:`V4L2_MEOMORY_MMAP ／ V4L2_MEMORY_USERPTR ／ V4L2_MEMORY_OVERLAY`
* union m
* `__u32 offset`:当 memory 类型是`V4L2_MEMORY_USERPTR`的时候，这是一个指向虚拟内存中 buffer 的指针，由应用程序来设定。
* `__u32 length`:buffer 的 size.

在 driver 内部管理 着两个 buffer queues ，一个输入队列，一个输出队列。

* 对于 capture device 来说，当输入队列中的 buffer 被塞满数据以后会自动变为输出队列，等待调用 VIDIOC_DQBUF 将数据进行处理以后重新调用 VIDIOC_QBUF 将 buffer 重新放进输入队列；
* 对于 output device 来说 buffer 被显示以后自动变为输出队列。

刚初始化的所有 map 过的 buffer 开始都处于 dequeced 的状态，由 driver 来管理对应用程序是不可访问的。

* 对于 capture 应用程序来说，首先是通过 VIDIOC_QBUF 将所有 map 过的 buffer 加入队列，然后通过 VIDIOC_STREAMON 开始 capture ，并进入 read loop ，在这里应用程序会等待直到有一个 buffer 被填满可以从队列中 dequeued ，当数据使用完后再 enqueue 进输入队列；
* 对于 output 应用程序来说，首先应用程序会 buffer 装满数据然后 enqueued ，当足够的 buffer 进入队列以后就调用 VIDIOC_STREAMON 将数据输出。 

有两种方法来阻塞应用程序的执行，直到有 buffer 能被 dequeued ，默认的是当调用 VIDIOC_DQBUF 的时候会被阻塞，直到有数据在 outgoing queue ，但是如果打开设备文件 的时候使用了 O_NONBLOCK ，则当调用 VIDIOC_DQBUF 而又没有数据可读的时候就会立即返回。另外一种方法是调用 select 和 poll 来对文件描述符进行监听是否有数据可读。

VIDIOC_STREAMON 和 VIDIOC_STREAMOFF 两个 ioctl 用来开始和停止 capturing 或者 output ，而且 VIDIOC_STREAMOFF 会删除输入和输出队列中的所有 buffer 。

因此 drvier 如果要实现 memory mapping I/O 必须支持 VIDIOC_REQBUFS, VIDIOC_QUERYBUF, VIDIOC_QBUF, VIDIOC_DQBUF, VIDIOC_STREAMON 和 VIDIOC_STREAMOFF ioctl, the mmap(), munmap(), select() 和 poll() 函数 。

User Pointers 是一种综合了 Read/Write 和 memory mappded 优势的 I/O 方法， buffer 是由应用程序自己申请的，可以是在虚拟内存或者共享内存中。在 capture 和 output 方面基本来说和 memory mapped 方式是相同的，在这里只提一下它申请内存的方式。

User pointer 方式下，申请的内存也 memory page size 为单位对齐，而且 buffersize 也有一定限制，例示代码中是这样计算 buffer size 的，暂时还不知道这样分配 buffer size 的依据是什么，先简单地这样用就好了：

```c
page_size = getpagesize();
buffer_size = (buffer_size + page_size -1) & ~(page_size - 1);
buffers[n_buffers].start = memalign (/* boundary */ page_size,
buffer_size);
```

# 3. start_capturing

经过上面的一系列的数据协商已经 buffer 的分配以后就可以调用 VIDIOC_QBUF 将 buffer 全部加入输入队列中，并调用 VIDIOC_STREAM0N 开始捕获数据了：

```c
int ioctl(int fd, int request, struct v4l2_buffer *argp);
//VIDIOC_QBUF VIDIOC_DQBUF

int ioctl(int fd, int request, const int *argp);
//VIDIOC_STREAM0N VIDIOC_STREAMOFF （ int 参数是 buffer 类型）
```

# 4.mainloop

开始捕获数据以后就会进入一个主循环，可以使用 select 或者 poll 来监听文件描述符的状态，一旦有数据可读，就调用函数来读取数据。

# 5.read_frame

读取数据根据 I/O 方式的不同而不同：

* Read/Write 方式直接从文件描述符中读一个帧大小的数据；
* Memory mapped 方式下先从输出队列中 dequeued 一个 buffer ，然后对帧数据进行处理，处理完成以后再放入输入队列。
* User pointer 方式下也是首先从输出队列中 dequeued 一个 buffer ，然后对这个 buffer 进行判断，看是否是应用程序开始申请的 buffer ，然后再对这个 buffer 进行处理，最后放入输入队列。

# 6. stop_capturing / uninit_device / close device

最后就是捕捉以及资源释放并关闭 device 。









