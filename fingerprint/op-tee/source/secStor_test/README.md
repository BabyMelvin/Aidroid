	本TA和CA主要是实现OP-TEE中的secure storage的使用，secure sotrage
主要是通过对persistentObject的操作来实现的，本TA和CA的source code
中，在CA的source code中，可以通过调用相关的API来实现对存放在/data/tee
目录中的使用secure storage保存的文件的读取，写入，重命名，删除，裁剪
创建等操作。
	将TA和CA集成到了系统中之后，本CA中提供了一个测试的demo,具体操作如下：
在linux shell端执行“secStorTest”命令就可以，在CA的main函数将会使用secure 
storage来完成文件的创建，数据写入，数据读出，文件裁剪，文件重命名和文件
删除操作。
