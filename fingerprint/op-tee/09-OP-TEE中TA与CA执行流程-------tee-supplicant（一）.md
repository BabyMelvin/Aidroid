`tee_supplicant`的主要作用是使OP-TEE能够通过tee_supplicant来访问**REE端文件系统中的资源**。例如：

* 1.加载存放在**文件系统中的TA镜像**到TEE中
* 2.对**REE端数据库**的操作，对**EMMC中RPMB分区**的操作，提供socket通信等。

其源代码`optee_client/tee-supplicant`目录中。编译之后会生成一个名字为`tee_supplicant`的可执行文件，该可执行文件在REE启动的时候会作为一个后台程序被自动启动，而且常驻于系统中。

# 1.1. tee_supplicant编译生成和Linux中的自启动
tee_supplicant会在编译`optee-client`目标的时候被编译生成一个可执行文件，具体编译过程请查看optee-client目录中的Makefile文件结合《3.OP-TEE+qemu的编译--工程编译target依赖关系》文章就能明了该可执行文件是如何一步步被编译出来的。

　`tee_supplicant`可执行文件在Linux启动的时候会被作为后台程序启动。启动的动作存放在`build/init.d.optee`文件中，其内容如下：

```bash
#!/bin/sh

#/etc/init.d/optee

#start/stop tee-supplication (OP-TEE normal world daemon)

case "$1" in
	start)
		if [ -e /bin/tee-supplicant -a -e /dev/teepriv0 ];then
			echo "Starting tee-supplicant ..."
			tee-supplication &
			exit 0
		else
			echo "tee-supplicant or TEE device not found"
			exit 1
		fi
		;;
	stop)
		killall tee-supplicant
		;;
	status)
		# “||”之前为yes则后面执行
		cat /dev/teepriv0 2>&1 | grep -1 "Device or resource busy" || not = "not "
		echo "tee-supplicant is ${not}active"
		;;
esca
```
在编译的时候init.d.optee文件将会被打包到根文件系统中并以"optee“名字存放在`/etc/init.d`目录中。而且会被链接到`/etc/rc.d/S09_optee`文件。这些操作是在编译生成rootfs的时候所做的，详细情况请查看`build/common.mk`文件中`filelist-tee-common`目标的内容。系统启动`tee_supplicant`的过程如下图所示：

![](image/20170608121459754.png)

# 2. tee_supplicant入口函数

tee_supplicant启动后作为Linux中的一个后台程序运行，起到处理RPC请求 service的作用。通过类似于C/S的方式为OP-TEE提供对REE端文件系统的操作。该可执行文件的源代码的入口函数存放在`optee_client/tee-supplicant/src/tee_supplicant.c`文件中。其入口函数内容如下:

```cpp
int main(int argc,char*argv[]){
	struct thread_arg arg={.fd=-1};
	int e;
	/*初始化互斥体*/
	e=pthread_mutex_init(&arg.mutex,NULL);
	if(e){
		EMSG("pthread_mutex_init:%s",strerror(e));
		EMSG("terminating...");
		exit(EXIT_FAILURE);
	}

	/*判定是否带有启动参数，如果带有启动参数，则打开对应的驱动文件
    如果没有带参数，则打开默认的驱动文件*/
	if (argc > 2)
		return usage();
	if (argc == 2) {
		arg.fd = open_dev(argv[1]);
		if (arg.fd < 0) {
			EMSG("failed to open \"%s\"", argv[1]);
			exit(EXIT_FAILURE);
		}
	} else {
/*打开/dev/teepriv0设备，该设备为tee驱动设备文件，返回操作句柄*/
		arg.fd = get_dev_fd();
		if (arg.fd < 0) {
			EMSG("failed to find an OP-TEE supplicant device");
			exit(EXIT_FAILURE);
		}
	}
	if (tee_supp_fs_init() != 0) {
		EMSG("error tee_supp_fs_init");
		exit(EXIT_FAILURE);
	}
 
	if (sql_fs_init() != 0) {
		EMSG("sql_fs_init() failed ");
		exit(EXIT_FAILURE);
	}	
	/*调用process_one_request函数接收来自TEE的请求，并加以处理*/
	while (!arg.abort) {
		if (!process_one_request(&arg))
			arg.abort = true;
	}
 
	close(arg.fd);
	return EXIT_FAILURE;
}
```

# 3.tee_supplicant中的loop循环
tee_supplicant启动之后最终会进入一个loop循环，调用process_one_request函数来`监控`，`接收`，`处理`，`回复`OP-TEE的请求。整个处理过程如下图所示：

![](image/20170608121635411.png)

process_one_request函数的内容如下：

```cpp
static bool process_one_request(struct thread_arg *arg){
	union tee_rpc_invoke request;
	size_t num_params;
	size_t num_meta;
	struct tee_ioctl_param *params;
	uint32_t func;
	uint32_t ret;
	
	DMSG("looping");
	memset(&request,0,sizeof(request));
	request.recv.num_params=RPC_NUM_PARAMS;
	
	/*let it be known that we can deal with meta parameters*/
	/*组合 tee_supplican等待TA请求的参数*/
	params=(struct tee_ioctl_param*)(&request.send+1);
	params->attr=TEE_IOCTL_PARAM_ATTR_META;
	
	/*增加当前正在在处理的tee_supplicant的数量*/
	num_waiters_inc(arg);
	
	/*通过ioctl函数，将等待请求发送到tee驱动，在tee驱动驱动中将会block住，直到有来自TA的请求才会返回*/
	if(!read_request(arg->fd,&request)){
		return false;
	}
	/*解析从TA发送的请求，分离出TA需要tee_supplicant所做的事情ID和相关参数*/
	if(!find_params(&request,&func,&num_params, &ms,&num_meta)){
		return false;
	}
	/*创建新的线程来等待接收来自TA的请求，将等待请求的数量减一*/
	if(num_meta && !num_waiters_dec(arg) && !spawn_thread(arg)){
		return false;
	}
	/*根据TA请求的ID来执行具体的handle*/
	switch(func){
		case RPC_CMD_LOAD_TA:
			//加载在文件系统的TA镜像
			ret=load_ta(num_params,params);
			break;
		case RPC_CMD_FS:
			//处理操作文件系统
			ret=tee_supp_fs_process(num_params,params);
			break;
		case RPC_CMD_SQL_FS:
			//处理操作数据库
			ret=sql_fs_process(num_params,params);
			break;
		case RPC_CMD_RPMB:
			//处理对EMMC中的rpmb分区
			ret=process_rpmb(num_params,params);
			break;
		case RPC_CMD_SHM_ALLOC:
			//处理分配共享内存的请求
			ret = process_alloc(num_params,params);
			break;
		case RPC_CMD_SHM_FREE:
			//释放分配的共享内存请求
			ret=process_free(num_params,params);
			break;
		case RPC_CMD_GPROF:
			//处理gprof请求
			ret=gprof_process(num_params,params);
			break;
		case OPTEE_MSG_RPC_CMD_SOCKET:
			//处理网络socket请求
			ret=tee_socket_process(num_params,params);
			break;
		default:
			EMSG("Cmd [0x%" PXI 32 "] not supported",func);
			ret = TEEC_ERROR_NOT_SUPPORTED;
			break;
	}
	request.send.ret =ret;
	//处理后的数据给 TA
	return write_response(arg->fd,&request);
}
```

# 4.接收来自TA的请求
`tee_supplicant`通过`read_request`来接收来自**TA端**的请求。该函数会block在tee驱动层面。内容如下：

```cpp
static bool read_request(int fd,union tee_rpc_invoke *request){
	struct tee_ioctl_buf_data data;
	data.buf_ptr=(uintptr_t)request;
	data.buf_len=sizeof(*request);
	/*将在tee_supplicant中设定的用于存放TA请求的buffer和属性的地址作为参数，
    然后调用ioctl函数进入到tee驱动中等待来自TA的请求*/
	if(ioctl(re,TEE_IOC_SUPPL_RECV,&data)){
		EMSG("TEE_IOC_SUPPL_RECV: %s",strerror(errno));
		return false;
	}
	return true;
}
```
在OP-TEE驱动中ioctl的`TEE_IOC_SUPPL_RECV`操作将会**block住**，直到接收到来自`TA的请求`。关于驱动部分将在后续章节详细介绍。

# 5.解析来自TA的请求
在tee_supplicant中，使用find_params函数来解析来自TA的请求。函数内容如下：

```cpp
static bool find_params(union tee_rpc_invoke *request,uint32_t *func,size_t *num_params,struct tee_ioctl_params ** params,size_t *num_meta){
	struct tee_ioctl_param *p;
	size_t n;
	p=(struct tee_ioctl_param *)(&request->recv +1);
	/*Skip meta parameters in the front*/
	/*跳过属性为TEE_IOCTL_PARAM_ATTR_META的参数*/
	for(n=0;n<request->recv.num_params;n++){
		if(!p[n].attr & TEE_IOCTL_PARAM_ATTR_META){
			break;
		}
	}
	//记录TA请求操作编号
	*func=request->recv.func;
	//确定TA真正的参数个数
	*num_params = request->recv.num_params - n;
	//将param指向TA发送过来的参数
	*params= p +n;
	//定位meta的其实位置
	*num_meta = n;
	
	/*make sure that no meta parameters follows a non-meta parameter*/
	//确保剩下的参数中没有属性为TEE_IOCTL_PARAM_ATTR_META的参数
	for(;n<request->recv.num_params;n++){
		if(p[n].attr & TEEC_IOCTL_PARAM_ATTR_META){
			EMSG("Unexpected meta parameter");
			return false;
		}
	}
	return true;
}
```

# 6. 请求的处理
当解析玩来自TA的请求参数信息之后，在process_one_request函数中会使用switch方式，根据请求的func ID来决定具体执行什么操作，这些操作包括：

1. 从文件系统中读取TA的镜像保存在共享内存中。
2.  对文件系统中的节点进行读/写/打开/关闭/移除等操作。
3.  执行针对数据库的操作。
4.  执行RPMB相关操作。
5.  分配共享内存。
6.  释放共享内存。
7.  处理gprof请求。
8.  执行网络socket请求。

# 7.回复数据给TA
tee_supplicant执行完具体的操作请求之后，会通过`write_response`函数将执行结果和数据反馈给TA。该函数内容如下：

```cpp
static bool write_response(int fd,union tee_rpc_invoke* request){
	struct tee_ioctl_buf_data data;
	/*将需要返回给TA的数据存放在buffer中*/
	data.buf_ptr = (uintptr_t)&request->send;
	data.buf_len = sizeof(struct tee_iocl_supp_send_arg) +
		       sizeof(struct tee_ioctl_param) *
				request->send.num_params;
	
/* 调用驱动中ioctl函数的TEE_IOC_SUPPL_SEND功能，进数据发送给TA */
	if (ioctl(fd, TEE_IOC_SUPPL_SEND, &data)) {
		EMSG("TEE_IOC_SUPPL_SEND: %s", strerror(errno));
		return false;
	}
	return true;
}
```

# 8.tee_supplicant中使用的结构体
在tee_supplicant中用于**接收**和**发送**请求的的数据都存放在类型为`tee_rpc_invoke`的结构体变量中，该结构体内容如下:

```cpp
union tee_rpc_invoke{
	uint64_t buf[(RPC_BUF_SIZE -1)/sizeof(uint64_t)+1];
	struct tee_ioctl_supp_recv_arg recv;
	struct tee_ioctl_supp_send_arg send;
};
```
RPC_BUF_SIZE的定义如下：

```c

#define RPC_BUF_SIZE	(sizeof(struct tee_iocl_supp_send_arg) + \
			 RPC_NUM_PARAMS * sizeof(struct tee_ioctl_param))
```

整个结构体中成员的排列如下图所示：

```
struct tee_ioctl_supp_send_arg
struct tee_ioctl_param
struct tee_ioctl_param
struct tee_ioctl_param
struct tee_ioctl_param
struct tee_ioctl_param
```
在整个结构体中等待来自TA的请求的时候，第一部分为`tee_ioctl_supp_send_arg`结构体，当处理完请求之后，需要将处理后的数据发送给TA时，第一部分为`tee_ioctl_supp_send_arg`
