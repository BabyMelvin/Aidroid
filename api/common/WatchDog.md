# WatchDog
硬件WatchDog定时检测关键硬件是否正常工作.
framework层一个软件WatchDog看关键系统服是否发生死锁事件。是否处于Block状态。

* 监视reboot广播
* 监视mMonitors关键服务是否死锁。

## 1.WatchDog初始化

system_server进程启动初始化过程：

* 创建WatchDog对象(继承Thread)
* 注册reboot广播
* 调用start()开始工作
