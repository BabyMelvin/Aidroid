## 调用API

### 概述
下面例子说明如何使用API。C++代码创建一个JAVA VM并调用一个静态方法叫做`Main.test`.

```cpp
#include <jni.h>

JavaVM *jvm;
JNIEnv *env;
JavaVMInitArgs vm_args;
JavaVMOption* options=new JavaVMOption[1];
options[0].optionString="-Djava.class.path=/usr/lib/java";
vm_args.version=JNI_VERSION_1_6;
vm_args.nOptions=1;
vm_args.options=options;
vm_args.ignoreUnrecongnized=false;

//load and initialize a Java VM,return a JNI Interface

JNI_CreateJavaVM(&jvm,(void**)&env，&vm_args);
delete options;
//invoke the Main.test method using the JNI
jclass cls=env->FindClass("Main");
jmethodID mid=env->GetStaticMethod(cls,"test","(I)V");
env->CallStaticVoidMethod(cls,mid,100);
//we are done 
jvm->DestroyJavaVM();
```

### 1.Create the VM
`JNI_CreateJavaVM`函数初始化一个JVM,返回一个JNI接口指针。`JNI_CreateJavaVM`调用的线程，被称为是主线程。

### 2.attaching to the VM
JNI 接口指针(JNIEnv)只有当前线程有限。另一个线程需要获得JavaVM必选调用`AttachCurrentThread()`将自己附着在VM上，然后得到一个JNI接口。直到`DetachCurrentThread()`调用之前都是有效的。

附着线程应该有足够的空间来实现大量工作。线程栈空间是操作系统分配的。例如，栈尺寸通过`pthread_attr_t`在`pthread_create`参数中。

### 3.detaching from the VM
必选调用`DetachCurrentThread()`才能推出。如果java方法在调用栈中不能自己摆脱VM。

### 4、Unloading the VM

`JNI_DestroyJavaVM()`能够卸载一个javaVM。