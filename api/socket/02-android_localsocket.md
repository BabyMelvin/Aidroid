# Android localsocket

* LocalSocket:在Unix域名空间创建一个套接字（非服务端）。是对Linux中Socket进行了封装，采用JNI方式调用，实现进程间通信。具体就是Native层Server和Framework层Client之间进行通信，或在各层次中能使用`Client/Server`模式实现通信。
* LocalServerSocket:创建服务器端Unix域套接字，与LocalSocket对应。
* LocalSocketImpl:Framework层Socket的实现，通过JNI调用系统socket API。
* LocalSocketAddress:Unix域socket的地址以及所处的空间。

JNI访问接口：`\frameworks\base\core\jni\android_net_LocalSocketImpl.cpp`

```
socket_create

socket_connect_local

socket_bind_local

socket_listen

……
```

socket API原本是为网络通讯设计的，但后来在socket的框架上发展出一种IPC机制，就是UNIX Domain Socket。虽然网络socket也可用于同一台主机的进程间通讯（通过loopback地址127.0.0.1），但是UNIX
 Domain Socket用于IPC更有效率：不需要经过网络协议栈，不需要打包拆包、计算校验和、维护序号和应答等，只是将应用层数据从一个进程拷贝到另一个进程。这是因为，IPC机制本质上是可靠的通讯，而网络协议是为不可靠的通讯设计的。UNIX Domain Socket也提供面向流和面向数据包两种API接口，类似于TCP和UDP，但是面向消息的UNIX Domain Socket也是可靠的，消息既不会丢失也不会顺序错乱。

UNIX Domain Socket是全双工的，API接口语义丰富，相比其它IPC机制有明显的优越性，目前已成为使用最广泛的IPC机制，比如X Window服务器和GUI程序之间就是通过UNIX Domain Socket通讯的。

使 用UNIX Domain Socket的过程和网络socket十分相似，也要先调用socket()创建一个socket文件描述符，address family指定为AF_UNIX，type可以选择SOCK_DGRAM或SOCK_STREAM，protocol参数仍然指定为0即可。

UNIX Domain Socket与网络socket编程最明显的不同在于地址格式不同，用结构体sockaddr_un表示，网络编程的socket地址是IP地址加端口号，而UNIX Domain Socket的地址是一个socket类型的文件在文件系统中的路径，这个socket文件由`bind()`调用创建，如果调用bind()时该文件已存在(已绑定)，则`bind()`错误返回。

GUI 系统中本机的客户/服务器结构通常基于 Unix Domain Socket 来实现。如X window 系统中，X11 客户在连接到 X11 服务器之前，首先根据 Display 等环境变量的设置来判断 X11 服务器所在的主机，如果主机是同一台主机，则会使用 UNIX Domain Socket 连接到服务器。

# Android应用程序使用Localsocket与Linux层通信
localsocket，也叫做Unix Domain Socket.在Android API中，有几个类对localsocket进行了封装，不仅可以用来应用程序之间进行IPC通信，还可以跨应用程序层和Linux层运行的程序进行通信。

假设一种情境，你的应用程序需要访问一些本身没有权限访问的数据或者设备，通过Java调用现有的命令行命令无法满足需求。可以考虑的方法是：将功能代码使用`C/C++`实现，然后编译成二进制文件。应用程序使用`Runtime.getRuntime().exec()`执行命令行命令，su提权后运行二进制文件，然后使用localsocket与之通信——`C/S模式`。当然，前提是手机要有root权限。
在Unix Domain Socket IPC已经贴过Server端代码，移植到Android编译不会出现太大的问题。其实Android已经对Linux socket进行了封装，直接使用更加方便。

## 1.java client
使用Java的Client端如下（仅关键代码）：
```java
LocalSocketAddress localSocketAddr = new LocalSocketAddress(sockFilePath, localSocketAddress.Namespace.FILESYSTEM);

LocalSocket socket = new LocalSocket();
String msg = "hello,world";
try {
  socket.connect(localSocketAddr);
  socket.setSendBufferSize(msg.length());

  OutputStream out = socket.getOutputStream();
  out.write(msg.getBytes());
  out.close();
  socket.close();
}catch(IOException e) {
    e.printStackTrace();
}
```

实例化一个LocalSocket对象之后，只需要简单的connect就行了。LocalSocket的connect函数接受一个LocalSocketAddress对象作为参数。

## 2.命名空间
LocalSocketAddress构造函数的第一个参数String name，是Server创建的socket文件的路径。需要注意的是第二个参数Namespace，有以下三种取值：

* `ABSTRACT`   ：A socket in the Linux abstract namespace 
* `FILESYSTEM` ：A socket named with a normal filesystem path. 
* `RESERVED` 	 ：A socket in the Android reserved namespace in `/dev/socket`.
具体选择哪一个（RESERVED貌似是系统用的，我们只需要在ABSTRACT和FILESYSTEM中选择一个），要看Server中实现方式，但是Client和Server一定要统一！！！之前写的Server例子直接就是在文件系统上创建的一个socket文件，所以选择FILESYSTEM。

ABSTRACT何时使用可以参考：Linux manual- unix, AF_UNIX, AF_LOCAL – Sockets for local interprocess communication 大致上是Linux在内存中维护一个socket文件描述符集合，但是不会在文件系统中生成一个具体的socket文件。


## 3.Android中对Localsocket的封装

Android本身就大量使用到了Localsocket进行跨系统层次的通信，对Localsocket的一些常用操作进行了封装。具体细节和使用方法可以参考Android系统源码的以下文件（代码源码文件根目录）：

```
/system/core/include/cutils/sockets.h
/sysetm/core/libcutils/socket_local_server.c
/system/core/libcutils/socket_local_client.c
```
