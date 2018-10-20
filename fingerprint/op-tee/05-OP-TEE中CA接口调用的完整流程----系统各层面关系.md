二级厂商需要实现自有`feature`的时候，需要添加自己的TA和CA接口:

* 当CA部分的接口需要提供给APP层面调用的时候，一般做法是在`framwork`中建立特定的service，在service中调用CA接口。同时也会建立对应的client部分，client部分与service部分通过Binder的方式进行同行来实现触发CA接口调用的操作。
* 而对于APP层面，开发者会将client部分提供的接口以JNI的方式进行封装暴露给上层APP调用。

当上层APP调用封装好的JNI接口之后，CA接口是如何通知特定的TA执行特定操作的呢？CA与TA之间的数据是如何传递的呢？`cortex`的状态是如何切换的呢？本文将详细讲解一次CA接口调用后功能实现的完整流程（本文不讨论FIQ和IRQ的情况，只讨论通过调用`SMC`指令调用到TEE的情况）。

# 1.支持TEE的系统各层面关系图
下图为层面结构图：

![](image/20170511210006850)

当CA接口处于`userspace`层面，整个调用过程需要经过`kernel spac`e层面， `linux driver`层面， `Monitor`态中断处理层面， `TEE OS kernel`层面， TEE的userspace层面（TA）。

# 2.userspace到kernelspace
在userspace 层面调用CA接口之后会触发system call操作，系统调用会将Linux陷入内核态，此时系统处于kernel space，然后根据传入的参数，找到对应的TEE driver，整个过程的过程大致如下：

![](image/20170511210145320)
关于该部分更加详细的内容，例如：软中断如何在kernel中注册的，系统调用的定义等等可查找系统调用（systemcall）和驱动的相关资料进行了解

# 3.TEE driver到monitor态

在TEE的driver中，最终会调用**SMC**这条汇编指令来实现与OP-TEE的沟通。当调用SMC汇编指令之后，会触发SMC软中断，该软中断的处理在`Cortex`的`Monitor`态中进行，随着ARM提供了ATF（ARM trust firmware），SMC的软中断具体处理函数在ATF中被实现，该软中断会根据发送指令是属于安全侧请求还是非安全侧的请求来控制是将`cortex`切换到`secure world态`还是`non-secure world态`。该切换动作在ATF的SMC中断处理函数中被实现。

# 4.TEE kernel space到TEE userspace

当SMC中断处理函数完成了将cortex的状态切换到secure world态以及相关参数的拷贝动作之后，TEE OS将接管剩下的操作。TEE OS首先将会获取从CA端传递过来的数据，然后解析出数据中写入的TA的UUID，然后查找对应的TA image是否被挂载到了 TEE OS中。

如果**没有TEE OS**将会与常驻在linux中的tee_supplicant进程通信，从`文件系统`中获取到`TA image`文件，并传递給TEE OS，然后加载该TA image。处理完整之后，TEE OS会切换到TEE userspace态，并将CA传递过来的其他参数传给具体的TA process，TA process获取到参数后，首先需要解除出参数中的commond ID值，根据具体的command ID值来做具体的操作。

当然TEE OS在加载TA image的时候会有`电子验签操作`来验证TA image的合法性。上述步骤的大致流程图如下：

![](image/20170511210437602)

由于一次完整的CA调用过程需要做一次调用执行`initContext`, `Opensession`, `InvokeCommand`, `close session`, `FinalizeContext`操作，所以上图只是大致流程，关于详细部分将会在后期章节中节后代码逐步讲解。

# 5.GP规定的C端接口

在GP中规定的CA端接口较少，主要的是五个接口，分别如下：

* `TEEC_InitializeContext`: 初始化TEE context，完成`open tee driver`，建立与TEE之间的context
* `TEEC_OpenSession`: 建立CA与TA之间的会话窗口
* `TEEC_InvokeCommand`: 想TA发送执行请求来执行具体的操作
* `TEEC_CloseSession`: 关闭CA与TA之间的会话窗口
* `TEEC_FinalizeContext`:清空建立的contex