# OpenMax多媒体框架
## 1. OpenMax基本层次结构
2006年，NVIDIA和Khrons推出OpenMax，这是多媒体应用程序的框架标准。OpenMax IL(继承层）技术规格定义了多媒体组件接口，以便嵌入式器件的多媒体框架中快速集成`加速编码`/`解码器`。

在Android中，`OpenMax IL层`通常用在多媒体引擎`插件`中，Android多媒体引擎`OpenCore`和`StrageFright`都可以使用**OpenMax作为插件**，主要用于编码和解码(Codec)处理。

在Android框架层中定义了由Androdi封装OpenMax接口，此接口和标准接口类似，但是使用的是C++类型接口，并且使用了Android的`Binder IPC`机制。
* `StageFright`使用Android封装的OpenMax接口.
* `OpenCore`没有使用此接口，而是使用其他形式对OpenMaxIL层接口进行封装。

<image src="image/01-01.png"/>

## 2.分析OpenMax框架构成
### 2.1 OpenMax总体层次结构
OpenMax分为三个层次，自上而下别为OpenMax DL(开发层)、OpenMax IL(集成层)和OpenMax AL(应用层)。在实际的应用中，OpenMax的三个层次中使用较多的是`OpenMax IL`集成层。由于操作系统和硬件的差异和多媒体应用的差异，OpenMax的DL和AL层使用相对较少。

* OpenMax DL(development layer)：在OpenMax DL中定义了一个API，这是音频、视频和图像功能的集合。OpenMax DL具备音频信号的处理功能，例如FFT和Filter，也具备图像原始处理功能，例如颜色空间转换、视频原始处理，并且可以实现如MPEG-4、H.264、Mp3、AAC和JPEG等编码/解码器的优化。
	* 硅供应商可以在一个新的处理器上实现并优化
	* 编码/解码供应商使用它来编写更广泛的编码/解码器功能。

* OpenMax IL(Integration Layer集成层)：是一种音频、视频和图像编码/解码器，能够实现和多媒体解码/编码的交互，并以统一的行为支持组件(例如资源和皮肤).这些编码/解码器或许是软件、硬件的混合体，其中可以将对用户是透明的底层接口应用于嵌入式、移动设备，这些接口包括应用程序和媒体框架。S编码/解码器供应商必须具备写私有的或者封闭的接口，这样就可以集成进移动设备。IL的主要目的的使用特征集合为编码/解码器提供一个系统抽象，解决多个不同的媒体系统之间的轻便性问题。
* OpenMax AL(Application Layer，应用层)OpenMax AL API在应用程序和多媒体中间件之间提供了一个标准化接口，多媒体中间件提供服务以实现被期待的API功能。OpenMax具有三个层次

<image src="image/01-02.jpg"/>

OpenMax API将会与处理器一块被提供，目的是使库和编码器/解码器开发者能够高速有效地利用新器件的完整加速潜能，而无须担心其底层的硬件结构，改标准是针对嵌入式设备和移动设备的多媒体软件架构，在架构底层上为多媒体的解码/编码器和数据定义了一套统一的编程接口，对多媒体数据的处理功能进行系统抽象，为用户屏蔽了底层的细节，因此，多媒体应用程序和多媒体框架通过OpenMax IL可以用一种统一的方式使用编码/解码器和其他多媒体数据处理能力，具有跨越软硬平台的可移植性。

## 2.OpenMax IL层的结构
在当前多媒体领域，OpenMax IL实际已经成为多媒体框架标准。大多数嵌入式处理器或者多媒体解码器、编码器模块的硬件生产者，通常提供标准的OpenMax IL层的软件接口，这样软件的开发者就可以基于此层次的标准化接口进行多媒体程序的开发。

OpenMax IL的接口层次比较科学，既不是硬件编码/解码器的接口，也不是应用程序层的接口，所以比较容易低实现标准化，OpenMax IL的层次结构如图:

<image src="image/01-03.png"/>

层次结构中，虚线部分内容是OpenMax IL层的内容，功能实现OpenMax IL中各个组件(Component)
* 对于下层而言，OpenMax IL既可以调用OpenMax DL层接口，也可以直接调用各种Codec实现。
* 对上层而言，OpenMax IL既可以被OpenMax AL层框架（Midddleware）调用，也可以被应用程序直接调用。

OpenMax IL层中包含主要内容如下：

* 客服端（Client)：OpenMax IL的调用者。
* 组件(Component)：OpenMax IL的单元，每一个组件实现一种功能。
* 端口(Port)：组件输入/输出接口。
* 隧道化(Tunneled):让两个组件直接连接的方式。

OpenMax IL层的运作流程:

<image src="image/01-04.png"/>
其中，OpenMax IL层客户端通过调用如下4个`OpenMax IL`组件来实现同一个功能。

* `Source组件`：只有一个输出端口。
* `Host组件`：有一个输入端口和一个输出端口。
* `Accelerator组件`：具有一个输入端口，调用了硬件的编码/解码器，加速体现在此环节。
* `Sink组件`：`Accelerator组件`和`Sink组件`通过私有通信方式在内部进行连接，没有经过明确的组件端口。

在使用OpenMax IL时候，有好几种处理数据流方式，既可以经过客服端，也可以不经过客户端。图中

* Source 组件到Host组件的数据流就是经过客户端的；
* 而Host组件到Accelerator组件的数据流就没有经过客服端，使用了隧道化的方式；
* Accelertor组件和Sink组件甚至可以使用私有的通信方式。

`OpenMax Core`是**辅助组件正常运行的模块**，完成各个组件的初始化等工作，真正运行过程中，重点是各个OpenMax IL组件，OpenMaxCore不是重点，也不是标准。

在OpenMax IL层中，正真核心内容是OpenMax IL组件，此组件以输入、输出端口为接口，端口可以被连接到另一个组件上，外部对组件可以发送命令，还可以进行设置/获取参数、配置等内容。组件的端口可以包含缓冲区(Buffer)的队列。

在OpenMax IL层中，组件的处理的核心内容是通过输入端口技术来消耗Buffer，通过输出端口来填充Buffer，由此多组件相连接就可以构成流式的处理。在OpenMAX IL层中，一个组件基本结构如图：

<image src="image/01-05.png"/>
组件功能和定义的端口类型有着千丝万缕的联系。通常情况下，
* 只有一个输出端口的是Source组件；
* 只有一个输入端口的是Sink组件；有多个输入端口、一个输出端口的是Mux组件；
* 有一个输入端口、多个输出端口的是DeMux组件
* 输入和输出端口各一个组件的为中间处理环节，这是最常见的组件。

端口根据应用来支持不同的数据类型。例如存在输入、输出端口各一个组件，其输入端口使用MP3格式的数据，输出端口使用PCM格式的数据，那么此组件就是一个MP3解码组件。

通常隧道化（Tunneled)可以将不同的组件的一个输入端口和一个输出端口连接在一起，在这种情况下，两个组件的处理过程合并，共同处理，尤其对于单输入和单输出的组件，两个组件将作为类似一个使用。

**注意**：隧道化是一个关于组件连接方式的概念。

## 3.Android 中的OpenMax
Android 系统中，主要使用时标准OpenMaxIL层的接口，其中只是简单的封装，标准的OpenMaxIL实现可以很容易地以插件形式加入到Android系统中。

在Android多媒体引起OpenCore和StageFright中，都可以使用OpenMax作为多媒体编码/解码器的插件，但是并没有直接使用OpenMAX IL层提供的纯C接口，而是进行一定封装。

Android系统对OpenMax支持的力度逐渐扩大。在Android2.x版本之后，Android的框架层开始封装定义OpenMax IL层接口，甚至使用Android中的Binder IPC机制来使用。在Stagefright中使用了OpenMax IL层接口，但是没有事会用OpenCore。OpenCore使用OpenMax IL层作为编码/解码器插件。

在Android系统中，主要使用了OpenMax的编码/解码器功能。虽然OpenMax也可以使用生成输入、输出、文件解析/构建等组件，但是Android使用最多的依旧是编码/解码器组件。媒体输入、输出环节和系统有很多关系，引入OpenMax标砖会比较麻烦：文件解析、构建环节一般不需要使用硬件加速。因为编码/解码器组件最能体现硬件加速环节，所以最常使用。

在Android系统中实现OpenMax IL层和标准的OpenMax IL层，一般需要实现如下两个环节：

* 编码/解码器启动程序：位于 Linux空间，需要通过Linux内核调用驱动程序，通常使用非标准的驱动程序。
* OpenMax IL层：根据OpenMax IL层的标准头文件实现不同的功能的组件。

另外，在Android中还提供了OpenMax的适配层接口（对OpenMax IL的标准组件进行封装适配），它作为Android本地的接口，可以被Android的多媒体引擎调用。