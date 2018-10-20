# android在init.rc中配置socket及使用

一般的native和framework的通信是通过jni，但是这一般只是framework调用native，native如果有消息要怎样通知上层呢？android中GSP模块提供一种解决思路，但是实现有些复杂，这里介绍一种使用socket通信的方法可以使native和framework自由通信，具体实现如下：

* android中使用jni对linux中的socket进行了封装。使用起来十分的方便。由于android是基于linux的，所以linux的代码会在java之前先执行，所以一般native端是服务器。framework端是客户端。

## 1.java层主要代码

```java
LocalSocket s=null;
LocalSocketAddress l;
s = new LocalSocekt();
l = new LocalSocektAddress(SOCKET_NAME,LocalSocketAddress.Namespace.RESERVED);
s.connect(l);
```
到此时如果socket连接没有问题，就可以像正常的读写了。native层主要代码：

```cpp
s_fdListen = android_get_control_socket(SOCKET_NAME);
ret = listen(s_fdListen,n);
s_fdCommand = accept(s_fdListen,(sockaddr *) &peeraddr, &socklen);
```
如果连接没有问题就可以使用linux中的`write/read`来对socket进行读和写了；

### 1.1SOCKNAME说明
这里有必要解释一下`SOCKET_NAME`，它的值是一个字符串，它在init.rc中定义的一个字符串。也就是说，**我们可以通过修改init.rc中来申请我们需要的socket资源**。
这里以ril为例来说明：

```
service ril-daemon /system/bin/rild
	socket rild stream 660 root radio
	socket rild-debug stream 660 radio system
	user root
	group radio cache inet misc audio
```
他的作用是由init.c来解析init.rc，并为我们启动一个名为`rild`的守护进程，它是一个可执行程序，我们通过adb shell在`system/bin`中可以找到对应的rild文件。

socket表示为这个守护进程分配一个socket资源，这个socket资源可以在`/dev/socket/`下找到rild。也就是本文要这里最关键的地方，socket能不能通就看守护进程能不能很好的起来。上面SOCKET_NAME也就是这里定义的字符串（在ril.java和ril.cpp中就有一个字符串常量SOCKET_NAME_RIL，他的值就是rild，和上面的对应）。

## 1.2 自定义
如果我们要自定义一个socket来进行通信，我们可以在init.rc的最后面加上。

```
service myserver-daemon /system/bin/server
	socket server stream 666
	oneshot
```

`system/bin/server`就是我们编译生成的服务器程序，在里面我们调用:

```cpp
s_fdListen = android_get_control_socket("server");
ret = listen(s_fdListen,n);
s_fdCommand=accept(s_fdListen,(sockaddr *) &peeraddr, &socklen);
```
就可以建立一个服务器端程序。java只需要使用最上面的代码就可以和native通信了，注意SOCKET_NAME值必须上下统一和init.rc中的相等，此处为“rild”。这里的oneshot必须有，没有的话，你的server很可能起不来。