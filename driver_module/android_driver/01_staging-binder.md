# Goldfish下的驱动解析
Android 驱动分为`专用驱动`和`设备驱动`两大类，其中Android的专用驱动不是Linux中标准内容，而是与体系结构无关和硬件平台无关的`纯软件`。Android专用驱动和Linux驱动类似，主要保存在`drivers/staging/android`目录中，只有极少数保存在其他目录中。

在移植中，无需做出任何更改即可进行配置，并灵活选择使用程序。

<image src="image/01-01.png"/>

## 1.staging驱动概述
保存目录`drivers/staging/android`目录中，Android 所特有目录。还包含`Makefile`和`Kconfig`文件，其中Makefile文件：

```makefile
#binder 和 logger两个普通的misc驱动程序
obj-$(CONFIG_ANDROID_BINDER_IPC) +=binder.o
obj-$(CONFIG_ANDROID_LOGGER)+=logger.o
# 利用控制台驱动的框架
obj-$(CONFIG_ANDROID_RAM_CONSOLE)+=ram_console.o
# 一种android特有的驱动程序框架
obj-$(CONFIG_ANDROID_OUTPUT)+=timed_output.o
# 基于timed_output的一个驱动程序
obj-$(CONFIG_ANDROID_TIMED_GPIO)+=timed_gpio.o
# 内存管理的组件
obj-$(CONFIG_ANDROID_LOW_MEMORY_KILLER)+=lowmemorykiller.o
```

## 2.Binder驱动程序
Android可选择的进程通信方式主要三种：

* 标准Linux Kernel IPC接口
* 标准D-BUS接口
* Binder接口

使用最多的是Binder，Binder更加简洁和快速，消耗内存资源更少。Binder解决了传统进程通信带来的额外开销问题，避免进程过载和安全漏洞等方面风险。Binder主要以下实现功能：

* 用驱动程序来推动进程间的通信
* 通过共享内存来提高性能。
* 为进程请求分配每个进程的线程池。
* 针对传统中对象引入了引用技术和跨进程的对象引用映射。
* 进程间同步调用。

## 2.1 Binder驱动的原理
Binder采用AIDL(android interface definition language)来描述进程间接口。Binder作为一个特殊字符设备而存在，设备节点`/dev/binder`由两个文件实现：

```
kernel/drivers/staging/binder.h
kernel/drivers/staging/binder.c
```
驱动实现过程中，使用`binder_ioctl()`函数与用户空间进程交换数据。`BINDER_WRITE_READ`用来读写数据，数据包中的cmd域用于区分不同的请求。

使用`binder_thread_write()`函数来发送请求的返回结果。

* 通过`binder_transaction()`函数来转发请求并返回结果。当收到请求时，`binder_transaction()`函数会通过对象的handle找到对象handle找到对象所在进程。如果handle为空，则认为此对象是`context_mgr`,然后把请求发给`context_mgr`所在进程。并将请求所有的Binder对象全部放在一个RB树种，最后把请求放到目标进程的队列中等待目标进程读取。在`binder_parse()`中实现数据解析工作。 
使用`binder_thread_read()`来读取结果。

## 2.2 Binder的工作流程

Binder工作流程如下：

* 1.客服端首先获得服务端代理对象。所谓代理对象实际是客服端建立一个服务端“引用”，该代理对象具有服务端功能，使其客服端访问服务器方法就像访问本地方法一样。
* 2.客服端通过调用服务器代理对象方式向服务器发送请求。
* 3.代理对象将用户请求的Binder驱动发送到服务器进程
* 4.服务器进程处理用户请求，并通过Binder驱动返回处理结果给客服端的服务器代理对象。
* 5.客服端收到服务端的返回结果。

## 2.3 实现Binder驱动
Binder实质是要把对象一个进程映射到另一个进程中，而不管这个对象是本地的还是远程的

### 2.3.1 binder_work
Binder中使用数据结构，然后分析整个流程：

```c
struct binder_work{
	struct list_head entry;
	enum{
		BIDNER_WORK_TRANSACTION=1,
		BINDER_WORK_TRANSACTION_COMPLETE,
		BINDER_WORK_NODE,
		BIDNER_WORK_DEAD_BINDER,
		BIDNER_WORK_DEAD_BINDER_AND_CLEAR,
		BINDER_WORK_CLEAR_DEATH_NOTITICATION,
	} type;
};
```
entry被定义为list_head，用来实现一个双向链表，用于存储所有`binder_work`队列；type表示binder_work的类型。

### 2.3.2 Binder 类型
用enum来表示Binder类型的定义代码：

```c
#define B_PACK_CHARS(c1,c2,c3,c4) \
 (((c1<<24)) | (((c2)<<16)) | (((c3)<<8) | (c4))))
#define B_TYPE_LARGE 0x85
enum{
	BINDER_TYPE_BINDER=B_PACK_CHARS('s','b','*',B_TYPE_LARGE),
	BINDER_TYPE_WAKE_BINDER=B_PACK_CHARS('w','b','*',B_TYPE_LARGE),
	BINDER_TYPE_HANDLE=B_PACK_CHARS('s','h','*',B_TYPE_LARGE),
	BINDER_TYPE_FD=B_PACK_CHARS('w','h','*',B_TYPE_LARGE),
};
```
上述代码中Binder被分成三个不同的大类，分别是本地对象(BINDER_TYPE_BINDER、BINDER_TYPE_WAKE_BINDER),远程对象的“引用”(BINDER_TYPE_HANDLE、BINDER_TYPE_WAKE_HANDLE)和文件(BINDER_TYPE_FD),对于文件BINDER_TYPE_FD类型，将文件映射到句柄上，根据此fd找到对应的文件，然后再目标进程中分配一个fd，最后把这个fd赋值给返回的句柄。

### 2.3.3 Binder对象
通常把传递的数据，称为`Binder`对象(Binder Object)，源代码中使用`flat_binder_object`结构体：

```c
struct flat_binder_object{
	unsigned long type;
	unsigned long flags;
	union{
		void *binder;
		signed long handle;
	};
	vodi *cookie;
};
```

### 2.3.3 binder_transaction_data
Binder传递的实际内容是通过另外一个结构体`binder_transaction_data`来表示的，所以从`flat_binder_object`看不到Binder对象所传递的实际内容，定义如下：

```c
struct binder_transaction_data{
	union{
		//目标对象句柄
		//Binder根据handle找到对应处理进程，并把此事件任务分配给一个线程
		//并且那个线程正执行ioctl的BINDER_WRITE_READ操作，等待一个请求
		size_t handle;
		//与handle对应，
		//请求方使用handle指出远程对象，响应方使用ptr来寻址，这样可以找到需要处理此事件的对象。
		void	*ptr;
		//handle和ptr之间解析关系就是Binder 驱动需要维护的任务
	}target;
	//表示target额外数据
	void *cookie;
	//一个命令，描述请求操作
	unsigned int code;
	//描述传输方式与flag_binder_object的flags字段对应
	unsigned int flags;
	//该继承pid和uid
	pid_t	sender_pid;
	uid_t   sender_uid;
	//数据大小字节数
	size_t	data_size;
	//数据偏移量字节数
	size_t	offsets_size;
	//正真的数据
	//ptr表示target->ptr对应数据
	//buf表示handle对应数据
	union{
		struct{
			const void *buffer;
			const void *offset;
		}ptr;
		uint8_t buf[8];
	}data;
};
```

此结构体是Binder核心。

### 2.3.4 binder_proc
结构体`binder_proc`用于保存调用Binder的各个进程或线程的信息：

```c
struct binder_proc{
	//实现双向链表
	struct hlist_node proc_node;
	//线程队列、双向链表、所有的线程信息
	struct rb_root threads;
	struct rb_root nodes;	
	struct rb_root refs_by_desc;
	struct rb_root refs_by_node;

	//进程ID
	int pid;
	struct vm_area_struct *vma;
	struct task_struct    *task;
	struct files_struct   *files;
	struct hlist_node deferred_work_node;
	int deferred_work;
	void* buffer;
	ptrdiff_t user_buffer_offset;
	struct list_haed buffers;
	struct	rb_root	free_buffers;
	struct 	rb_root allocated_buffers;
	size_t  free_async_space;
	struct page** pages;
	size_t buffer_free;
	uin32_t buffer_free;
	struct list_head todo;
	//等待队列
	wait_queue_head_t wait;
	//Binder 状态
	struct	binder_stats stats;
	struct  list_head delivered_death;
	//最大先线程
	int max_threads;
	int required_threads;
	int requested_threads_started;
	int ready_threads;
	//默认优先级
	long default_priority;
};
```

### 2.3.6 binder_node
结构体binder_node表示一个Binder节点，

```c
struct binder_node{
	int debug_id;
	struct binder_work work;
	union{
		struct rb_node rb_node;
		struct hlist_node dead_node;
	};
	struct binder_proc *proc;
	struct hlist_head refs;
	int internal_strong_refs;
	int local_weak_refs;
	int local_strong_refs;
	void __user *ptr;
	void __user *cookie;
	unsigned has_strong_ref:1;
	unsigned pending_strong_ref:1;
	unsinged has_weak_ref:1;
	unsinged pending_weak_ref:1;
	unsinged has_async_transaction:1;
	unsinged accept_fds:1;
	int min_priority:8;
	struct list_hread async_todo;
};
```

### 2.3.7 binder_thread
结构体binder_thread用于存储每一个单独线程信息：

```c
struct binder_thread{
	//属于哪一个Binder进程
	struct binder_proc *proc;
	//红黑树节点
	struct rb_node rb_node;
	int pid;
	//表示线程的状态
	int looper;
	//定义一个接受和发送进程和线程信息
	struct binder_transaction* transaction_stack;
	//创建一个双向链表
	struct list_head　todo;
	//返回错误信息代码
	uint32_t return_error;
	uint32_t return_error2;
	//一个等待队列头
	wait_queue_head_t wait;
	//用于Binder状态信息
	struct binder_stats stats;
};
```

### 2.3.8 binder_transaction
用于中转请求和返回结果，保存接收和发送的进程信息

```c
struct binder_transaciton{
	int debug_id;
	// 一个binder_work
	struct binder_work work;
	//from 和to_thread接收和发送的进程消息
	struct binder_thread *from;
	struct binder_transaction* from_parent;
	//是一个binder_proc类型结构体，其中包括flags,need_reply，优先级等数据结构
	struct binder_proc* to_proc;
	struct binder_thread *to_thread;
	struct binder_transaction *to_parent;
	unsigned nedd_reply:1;
	//binder缓冲区信息
	struct binder_buffer *buffer;
	unsigned int code;
	unsigned int flags;
	long priority;
	long saved_priority;
	uid_t sender_euid;
};
```

## 2.4 binder 函数
### 2.4.1 初始化函数binder_init()

具体实现如下:

```c
static int __init binder_init(void){
	int ret;
	//创建文件系统根节点，/proc/binder
	binder_proc_dir_entry_root=proc_mkdir("binder",NULL);
	//创建proc节点，/proc/binder/proc
	if(binder_proc_dir_entry_root)
		binder_proc_dir_entry_proc=proc_mkdir("proc",binder_proc_dir_entry_root);
	//注册misc设备,/dev/binder
	ret=misc_register(&binder_miscdev);
	//创建各文件
	if(binder_proc_dir_entry_root){
		create_proc_read_entry("state",S_IRUGO,binder_proc_dir_entry_root,binder_read_proc_state,NULL);
		create_proc_read_entry("stats",S_IRUGO,binder_proc_dir_entry_root,binder_read_proc_stats,NULL);
		create_proc_read_entry("transactions",S_IRUGO,binder_proc_dir_entry_root,binder_read_proc_transaction_s,NULL);
		create_proc_read_entry("transaction_log",S_IRUGO,binder_proc_dir_entry_root,binder_read_proc_transation_log,&binder_transaction_log);
		create_proc_read_entry("failed transaction_log",S_IRUGO,binder_pro_dir_entry_root,binder_read_proc_transaction_log,&binder_transaction_log_failed);
		
	}
	return ret;
}
//设备驱动接口来调用
device_initcall(binder_init);
```
设备节点`/dev/binder`，该节点由init进程在`handle_device_fd(device_fd)`函数中调用`handle_device_event(&uevent)`函数执行，其中`uevent-netlink`事件在`/dev/`目录下创建，最后，调用`create_proc_read_entry`创建以下文件:

```
/proc/binder/state
/proc/binder/stats
/proc/binder/transactions
/proc/binder/transaction_log
/proc/binder/failed_transaction_log
```

注册Binder驱动为Misc设备时，需要制定Binder驱动的misdevice:

```c
static struct miscdevice binder_miscdev={
	.minor =MISC_DYNAMIC_MINOR,
	.name="binder",
	.fops=&binder_fops
};
//binder 文件操作结构体
static struct file_operations binder_fops={
	.owner=THIS_MODULE,
	.poll=binder_poll,
	.unlocked_ioctl=binder_ioctl,
	.mmap=binder_mmaper,
	.open=binder_open,
	.flush=binder_flush,
	.release=binder_release
}；
```

### 2.4.2 函数binder_open()
打开Binder设备文件`/dev/binder`，在Android驱动中任何一个进程及其内所有线程都可以打开一个Binder设备：

```c
static int binder_open(struct *nodp,struct file*flip){
	struct binder_proc* proc;
	if(binder_debug_mask&BINDER_DEBUG_OPEN_CLOSED)
		printk(KERN_INFO "binder_open:%d:%d\n",current->group_leader->pid);
	//为binder_proc分配空间
	proc=kzallo(sizeof(*proc),GFP_KERNEL);
	if(proc==NULL)
		return -ENOMEM;
	//增加引用计数
	get_task_struct(current);
	//保存引用计数
	proc->tsk=current;
	//初始化binder_proc队列
	INIT_LIST_HEAD(&proc->todo);
	init_waitqueue_head(&proc->wait);
	proc->default_proiority=task_nice(current);
	mutex_lock(&binder_lock);
	//增加BINDER_STAT_PROC
	binder_stats.obj_created[BINDER_STAT_PROC]++;
	//添加到binder_proc哈希表
	hlist_add_head(&proc->proc_node,&binder_procs);
	//保存pid和private_data等数据
	proc->pid=current->group_leader->pid;
	INIT_LIST_HEAD(&proc->delivered_death);
	filp->private_data=proc;
	mutex_unlock(&binder_lock);
	//创建只读文件/proc/binder/proc/$pid
	if(binder_proc_dir_entry){
		char strbuf[1];
		snprintf(strbuf,sizeof(strbuf),"%u",proc->pid);
		remove_pro_entry(strbuf,binder_pro_dir_entry_proc);
		create_proc_read_entry(strbuf,S_IRUGO,binder_proc_dir_entry_proc,binder_read_proc_proc,proc);
	}
	return 0;
}
```

### 2.4.3 binder_release()
与binder_open相反，释放相关数据和关闭。

### 2.4.4 binder_flush()
在关闭一个设备文件描述符复制调用flush操作接口。通过调用一个workqueue来执行`BINDER_DEFERRED_FLUSH`操作以完成该flush操作，将最终处理交给`binder_defer_work()`

### 2.4.5 binder_poll()
函数poll()是非阻塞型I/O内核驱动实现，所有支持非阻塞I/O操作的设备都需要实现poll()函数。Binder的poll()函数仅支持设备是否可以非阻塞的读(POLLIN),在此有proc_work和thread_work两种等待任务。其他驱动的poll实现一样，也需要通过调用poll_wait()函数来实现。

### 2.4.6 binder_get_thread()
函数用于在threads队列中查找当前进程信息

### 2.4.7 binder_mmap()
把设备内存映射到用户空间进程地址空间中，这样就可以像操作用户内存那样操作设备内存。

### binder_ioctl()
是Binder核心部分内容，Binder功能通过ioctl命令来实现的。Binder一共有7个ioctl命令，定义在`ioclt.h`文件中。

```c
#define BINDER_WRITE_READ      _IOWR('b',1,struct binder_write_read)
#define BINDER_SET_IDLE_TIMEOUT		_IOW('b',3,int64_t)
#define BINDER_SET_MAX_THREADS	_IOW('b',5,size_t)
#define BINDER_SET_IDLE_PRIORITY 	_IOW('b',6,int) 
#define BINDER_SET_CONTEXT_MGR	_IOW('b'7,int)
#define BINDER_THREAD_EXIT	_IOW('b',8,int)
#define BINDER_VERSION		_IOW('b',9,struct binder_version)
```
