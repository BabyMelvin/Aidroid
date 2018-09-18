# 1.MediaPlayerService启动

是由init.rc文件解析启动。

```
service media /system/bin/mediaserver
	class main
	user media
	group audio camera inet net_bt net_bt_admin sdcard_r
	ioprio rt 4
```
启动media服务。可执行文件mediaserver.

```c
using namespace android;
int main(int argc,char**argv){
	sp<ProcessState> proc(ProcessState::self());
	sp<IServiceManager> sm=defatultServiceManager();
	AudioFlinger::instance();
	MediaPlayerService::instaniate();
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPoool();
}
```

MediaPlayerService.cpp中实例化改对象。

```cpp
defaultServiceManager()->addService(String16("media.player"),new MediaPlayerService());
```