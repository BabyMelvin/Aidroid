# Logger驱动程序
Logger驱动是一个轻量级log驱动，功能是为用户程序提供Log支持，此驱动通常作为一个工具使用。Logger有三个设备节点：

* `/dev/log/main`:主要的Log
* `/dev/log/event`:事件的Log
* `/dev/log/radio`:Modem部分的log

对于非用户或本组而言，Logger驱动程序的设备节点时可写而不可读的。