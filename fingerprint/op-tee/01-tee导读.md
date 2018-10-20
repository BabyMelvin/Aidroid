# trustzone与OP-TEE介绍
如果系统芯片支持**trustzone技术**且支持**TEE**，那么TEE将在系统中提供一个安全的运行环境，用于保护系统中的重要数据和安全操作。

通常一个完成的系统除了有`bootloader`和`kernel`，`recovery`以及上层`android`系统之外还需要有**TEE OS**。系统启动的时候使用`secure boot`功能来保护整个系统的image不被恶意篡改。`OP-TEE`是**借助trustzone技术实现的一种TEE方案**。而TEE又属于整个系统中的一个部分，可以看作是与`linux kernel`同一个层面。

安全问题的严重性， google规定在android 7.0之后要求厂商必须使用`TEE`来保护用户的`生物特征数据`（`指纹`，`虹膜`等）。

# trustzone状态和模式切换
为确保用户的数据的安全，ARM公司提出了trustzone技术，个人将trustzone理解为`cortex的虚拟化技术`。ARM在`AXI系统总线`上添加了一根**额外**的安全总线，称为`NS位`，并将cortex分为两种状态：`secure world`, `non-secure world`, 并添加了一种叫做`monitor的模式`.

cortex根据`NS的值`来判定当前指令操作是需要安全操作还是非安全操作，并结合自身是属于secure world状态还是non-secure状态来判定是否需要执行当前的指令操作。而cortex的secure world和non-secure world状态之间的切换由`monitor`来完成。

最近由于`ATF`（arm trusted firmware）的给出，cortex的状态切换操作都是在ATF中完成。当cortex处于`secure world`状态时，cortex会去执行`TEE`(Trusted execute enviorment) OS部分的代码，当cortex处于non-secure world状态时，cortex回去执行linux kernel部分的代码。**而linux kernel是无法访问TEE部分所以资源**，只能通过特定的`TA`(Trust Application)和`CA`(Client Application)来**访问TEE部分特定的资源**。

# trustzone厂家实现
TEE是基于trustzone技术搭建的安全执行环境，当cortex处于secure world态时，cortex执行的是TEE OS的代码。而当前全世界并未有一个统一的TEE OS，各家厂商和组织都有各自的实现方式，但是所有的方案的外部接口都会遵循GP（GlobalPlatform）标准。所有对于二级厂商来说，使用更加方便。当前具有自己TEE解决方案的厂商有：`高通的Qsee`, `Trustonic的tee OS`， `OP-TEE OS`， `opentee`, `海思`,`Mstar`， `VIA`，`豆荚科技`等，笔者使用过上述几家厂商中的大部分，**外部接口统一**，**只是TA的添加和加载时的校验有所区别**。

由于各厂商的TEE OS都属于闭源的，所以关于内部的`SMC响应机制`，`TEE OS内各进程的调度机制`，`TZPC的相关配置`等都无法详细了解。本文以OP-TEE为例。在qemu上搭建OP-TEE的运行环境，后续章节将会介绍添加自有TA,CA,工程编译，启动机制，等其他更加详细的内容。

# OP-TEE代码结构

该工程中各目录的作用介绍如下：

* `bios_qemu_tz_arm`: 在qemu平台中运行tz arm的bios代码，启动最初阶段会被使用到，用来加载kernel, OP-TEE os image, rootfs并启动linux kernel和OP-TEE OS
* `build`:这个工程的编译目录，里面包含了各种makefile文件和相关配置文件
* `busybox`:busybox的源代码，用于制作rootfs的使用被使用到
* `gen_rootfs`:存放制作rootfs时使用的相关脚本和配置文件
* `hello_work`:一个示例代码，目录下包含了`TA`和`CA`部分的代码，在Linux shell端运行hello_world指令，就能调用CA接口，最终会穿到TEE中执行对应的TA部分的代码
* `linux`:linux内核代码，在`driver/tee`目录下存放的是tee对应的驱动程序
* `optee_client`:包含了CA程序调用的userspace层面的接口库的源代码。其中tee_supplicant目录中的代码会被编译成一个Binary，该binary主要的作用是，当调用CA接口，需要加载TA image时，TEE OS通过该binary从文件系统中来获取TA image，并传递給TEE OS，然后再讲TA image运行到TEE OS中。
* `optee_os`:存放OP-TEE OS的源代码和相关文档
* `optee_test`:opentee的测试程序xtest的源代码，主要用来测试TEE中提供的各种算法逻辑和提供的其他功能
* `out`:编译完成之后输出目录（该目录编译完成之后才会生成）
* `qemu`:qemu源代码
* `soc_term`:在启动时与gnome-terminal命令一起启动终端,用于建立启动的两个terminal的端口监听，方便OP-TEE OS的log和linux kernel log分别输出到两个terminal中
* `toolchains`:编译时需要使用的toolchain