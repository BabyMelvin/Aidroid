`libteec库`是OP-TEE提供给用户在linux userspace层面调用的接口实现，对于**该部分每家芯片厂商可能不一样**。本文将以OP-TEE的实现方法为例进行介绍。

libteec代码的具体实现存放在`optee_client/libteec`目录下，OP-TEE提供给linux端使用的接口源代码的实现存放在`optee_client/libteec/src/tee_client_api.c`,如下图所示：

![](image/20170606171655064.png)

在OP-TEE中libteec提供给上层用户使用的API一共有10个，他们分别为：

* 1.`TEEC_Result TEEC_InitializeContext(const char *name, TEEC_Context *ctx)`
* 2.`void TEEC_FinalizeContext(TEEC_Context *ctx)`
* 3.`TEEC_Result TEEC_OpenSession(TEEC_Context *ctx, TEEC_Session *session,const TEEC_UUID *destination,uint32_t connection_method, const void *connection_data,TEEC_Operation *operation, uint32_t *ret_origin)`
* 4.`void TEEC_CloseSession(TEEC_Session *session)`
* 5.`TEEC_Result TEEC_InvokeCommand(TEEC_Session *session, uint32_t cmd_id,TEEC_Operation *operation, uint32_t *error_origin)`
* 6.`void TEEC_RequestCancellation(TEEC_Operation *operation)`
* 7.`TEEC_Result TEEC_RegisterSharedMemory(TEEC_Context *ctx, TEEC_SharedMemory *shm)`
* 8.`TEEC_Result TEEC_RegisterSharedMemoryFileDescriptor(TEEC_Context *ctx,TEEC_SharedMemory *shm,int fd)`
* 9.`TEEC_Result TEEC_AllocateSharedMemory(TEEC_Context *ctx, TEEC_SharedMemory *shm)`
* 10.`void TEEC_ReleaseSharedMemory(TEEC_SharedMemory *shm)`

上述10个API都按照GP标准进行定义，使用上述10个API能够满足用户在userspace层面的需求，在系统中这部分会被编译成`libteec库`的形式存在。下面一次介绍上述10个API的作用：

## 1.TEEC_InitializeContext

* `函数原型`:`TEEC_Result TEEC_InitializeContext(const char *name, TEEC_Context *ctx)`
* `函数作用描述`:初始化一个TEEC_Context变量，该变量用于CA和TEE之间建立联系。其中参数name是用来定义TEE的身份，如果该参数为NULL，则CA将会选择默认的TEE方案来建立联系。该API是第一次调用TA是不行执行的函数。
* `参数说明`：
	* `name`:指向TEE的名字，一般情况下该值设置成NULL，使其选择默认的TEE方案进行连接
	* `ctx`:指向一个类型为TEEC_Context的变量的地址，该变量会用于CA与TA之间连接和通信。
* `函数返回值：`
	* `TEEC_SUCCESS`: 初始化操作成功
	* 其他返回值表示初始化失败

函数实现（在`OP-TEE`厂商中的实现）如下：

```cpp
TEEC_RESULT TEEC_InitializeContext(const char* name,TEEC_Context *ctx){
	char devname[PATH_MAX];
	int fd;
	size_t n;
	if(!ctx){
		return TEEC_ERROR_BAD_PARAMETERS;
	}	
	/*
	调用teec_open_dev打开可用的TEE驱动文件，在打开的过程中会校验TEE的版本信息
    如果检查合法，则会返回该驱动文件的句柄fd, 然后将fd赋值给ctx变量的fd成员
	*/
	for(n=0;n<TEEC_MAX_DEV_SEQ；n++){
		snprintf(devname,sizeof(devname),"dev/tee%zu",n);
		fd=teec_open_dev(devname,name);
		if(fd >=0){
			ctx->fd= fd;
			return TEEC_SUCCESS;
		}
	}
	return TEEC_ERROR_ITEM_NOT_FOUND;
}
```

## 2.TEEC_FinalizeContext

* `函数原型`：`void TEEC_FinalizeContext(TEEC_Context *ctx)`
* `函数作用描述`：释放一个已经被初始化过的类型为TEEC_Context变量，关闭CA与TEE之间的连接。**在调用该函数之前必须确保打开的session已经被关闭了**。
* `参数说明`：
	* `ctx`: 指向一个类型为TEEC_Context的变量，该变量会用于CA与TA之间连接和通信。
* `函数返回值`：无
* 函数实现（在OP-TEE中的实现）如下：

```cpp
void TEEC_FinializeContext(TEEC_Context *ctx){
	/*调用close函数，释放掉tee驱动文件的描述符来完成资源释放 */
	if(ctx)
		close(ctx->fd);
}
```

## 3.TEEC_OpenSession

* `函数原型`：`TEEC_Result TEEC_OpenSession(TEEC_Context *ctx, TEEC_Session *session,const TEEC_UUID *destination,uint32_t connection_method, const void *connection_data,TEEC_Operation *operation, uint32_t *ret_origin)`
* `函数作用描述`：打开一个CA与对应TA之间的一个session，该session用于该CA与对应TA之间的联系，该CA需要连接的TA是由UUID指定的。session具有不同的打开和连接方式，根据不同的打开和连接方式CA可以在执行打开session的时候传递数据给TA，以便TA对打开操作做出权限检查。各种打开方式说明如下：
	* `TEEC_LOGIN_PUBLIC`：不需要提供，也即是`connectionData`的值必须为NULL
	* `TEEC_LOGIN_USER`： 提示用户链接，connectionData的值必须为NULL
	* `TEEC_LOGIN_GROUP`： CA以某一组的方式打开session，connectionData的值必须指向一个uint32_t的数据，该数据组的信息。在TA端将会对connectionData的数据进行检查，判定CA是否真属于该组。
	* `TEEC_LOGIN_APPLICATION`： 以application的方式连接，connectionData的值必须为NULL
	* `TEEC_LOGIN_USER_APPLICATION`： 以用户程序的方式连接，connectionData的值必须为NULL
	* `TEEC_LOGIN_GROUP_APPLICATION`： 以组应用程序的范式连接，其中connectionData需要指向一个uint32_t类型的变量。在TA端将会对connectionData的数据进行权限检查，查看连接是否合法。
* `参数说明`：
	* `context`: 指向一个类型为TEEC_Context的变量，该变量用于CA与TA之间连接和通信，调用`TEEC_InitializeContext`函数进行初始化。
	* `session`: 存放session内存的变量
	* `destination`:指向存放需要连接的TA的UUID的值的变量
	* `connectionMethod`:CA与TA的连接方式，详细请参考函数描述中的说明
	* `connectionData`:指向需要在打开session的时候传递給TA的数据
	* `operation`:指向`TEEC_Operation`结构体的变量，变量中包含了一系列用于与TA进行交互使用的buffer或者其他变量。如果在打开session时CA和TA不需要交互数据，则可以将该变量指向NULL
	* `returnOrigin`:用于存放从TA端返回的结果的变量，如果不需要返回值，则可以将该变量指向NULL
* 函数返回值：
	* `TEEC_SUCCESS`: 初始化操作成功
	* 其他返回值表示初始化失败
* 函数实现（在OP-TEE中的实现）如下：

```cpp
TEEC_Result TEEC_OpenSession(TEEC_Context *ctx,TEEC_Session *session,const TEEC_UUID*destination,
uint32_t connection_method,const void*connection_data,
TEEC_Operation *operation,uint32_t *ret_origin){
	/*定义一个buffer，用于存放在执行openseesion需要传递和接收TA数据*/
	uint64_t buf[sizeof(struct teec_ioctl_open_session_arg)+TEEC_CONFIG_PAYLOAD_REF_COUNT
	*sizeof(struct teec_ioctl_param))/sizeof(uint64_t)]={0};
	/*定义buf_data，指向buf变量，用于传递給ioctl函数调用到tee驱动执行opensession操作*/
	struct tee_ioctl_buf_data buf_data;
	/*定义参数，用于对初始化需要传递給TA的数据buffer*/
	struct tee_ioctl_open_session_arg* arg;
	struct tee_ioctl_param *params;
	TEEC_Result res;
	uint32_t eorig;
	/*CA与TA之间的共享buffer*/
	TEEC_SharedMemory shm[TEEC_CONFIG_PAYLOAD_REF_COUNT];
	int rc;
	
	(void)&connection_data;
	/*参数检查*/
	if(!ctx || !session){
		eorig = TEEC_ORIGIN_API;
		res= TEEC_ERROR_BAD_PARAMETERS;
		goto out;
	}
	/*指针赋值*/
	buf_data.buf_ptr = (uintptr_t)buf;
	buf_data.buf_len = sizeof(buf);
 
	arg = (struct tee_ioctl_open_session_arg *)buf;
	arg->num_params = TEEC_CONFIG_PAYLOAD_REF_COUNT;
	params = (struct tee_ioctl_param *)(arg + 1);
	/* 将uuid的值填充到buffer中 */
	uuid_to_octets(arg->uuid, destination);
	arg->clnt_login = connection_method;

	/*填充TEEC_Operation结构体变量*/
	res = teec_pre_process_operation(ctx, operation, params, shm);
	if (res != TEEC_SUCCESS) {
		eorig = TEEC_ORIGIN_API;
		goto out_free_temp_refs;
	}
	/*调用ioctl函数，穿透到tee驱动中的ioctl中执行TEE_IOC_OPEN_SESSION操作开始
    执行opensession操作*/
	rc = ioctl(ctx->fd, TEE_IOC_OPEN_SESSION, &buf_data);
	if (rc) {
		EMSG("TEE_IOC_OPEN_SESSION failed");
		eorig = TEEC_ORIGIN_COMMS;
		res = ioctl_errno_to_res(errno);
		goto out_free_temp_refs;
	}
	res = arg->ret;
	eorig = arg->ret_origin;
	if (res == TEEC_SUCCESS) {
		session->ctx = ctx;
		session->session_id = arg->session;
	}
	/*解析出从TA中返回的数据*/
	teec_post_process_operation(operation, params, shm);
out_free_temp_refs:
	teec_free_temp_refs(operation,shm);
out:
	if(ret_origin)
		*ret_origin=eorign;
	return res;
}
```

# 4.TEEC_CloseSession

* `函数原型`：`void TEEC_CloseSession(TEEC_Session *session)`
* `函数作用描述`：`关闭已经被初始化的CA与对应TA之间的session，在调用该函数之前需要保证所有的command已经执行完毕。如果session为NULL,则不执行任何操作。`
* 参数说明：
	* `session`: 指向已经初始化的session结构体变量
* 函数实现（在OP-TEE中的实现）如下：

```cpp
void TEEC_CloseSession(TEEC_Session *session){
	struct tee_ioctl_close_session_arg arg;
	if(!session){
		return;
	}
	arg.session=session->session_id;
	
/* 调用ioctl函数命中TEE_IOC_CLOSE_SESSION操作，通知TA执行close session */
	if (ioctl(session->ctx->fd, TEE_IOC_CLOSE_SESSION, &arg))
		EMSG("Failed to close session 0x%x", session->session_id);
}
```

# 5.TEEC_InvokeCommand

* `函数原型`：`TEEC_Result TEEC_InvokeCommand(TEEC_Session *session, uint32_t cmd_id,TEEC_Operation *operation, uint32_t *error_origin)`
* `函数作用描述`:通过cmd_id和打开的session，来通知session对应的TA执行cmd_id指定的操作。
* 参数说明：
	* `session`:指向已经初始化的session结构体变量
	* `cmd_id`：TA中定义的command的ID值，让CA通知TA执行哪条command
	* `operation`: 已经初始化的`TEEC_Operation`类型的变量，该变量中包含CA于TA之间进行交互的buffer,缓存的属性等信息
	* `error_origin`:调用`TEEC_InvokeCommand`函数的时候，TA端的返回值
* 函数实现（在OP-TEE中的实现）如下：

```cpp
TEEC_Result TEEC_InvokeCommand(TEEC_Session *session,uint32_t cmd_id,TEEC_Operation*operation,uint32_t *error_origin){
	/*定义调用invokecommand函数是存放参数和共享内存的buffer*/
	uint64_t buf[(sizeof(struct tee_ioctl_invoke_arg) +
			TEEC_CONFIG_PAYLOAD_REF_COUNT *
				sizeof(struct tee_ioctl_param)) /
			sizeof(uint64_t)] = { 0 };
	struct tee_ioctl_buf_data buf_data;
	struct tee_ioctl_invoke_arg *arg;
	struct tee_ioctl_param *params;
	TEEC_Result res;
	uint32_t eorig;
	TEEC_SharedMemory shm[TEEC_CONFIG_PAYLOAD_REF_COUNT];
	int rc;
 
	if (!session) {
		eorig = TEEC_ORIGIN_API;
		res = TEEC_ERROR_BAD_PARAMETERS;
		goto out;
	}
	/*组合调用TA的command时需要使用的参数信息*/
	buf_data.buf_ptr = (uintptr_t)buf;
	buf_data.buf_len = sizeof(buf);
 
	arg = (struct tee_ioctl_invoke_arg *)buf;
	arg->num_params = TEEC_CONFIG_PAYLOAD_REF_COUNT;
	params = (struct tee_ioctl_param *)(arg + 1);
 
	arg->session = session->session_id;
	arg->func = cmd_id;
 
	if (operation) {
		teec_mutex_lock(&teec_mutex);
		operation->session = session;
		teec_mutex_unlock(&teec_mutex);
	}
	/*填充operation中的params域，用于CA与TA之间数据传输*/
	res = teec_pre_process_operation(session->ctx, operation, params, shm);
	if (res != TEEC_SUCCESS) {
		eorig = TEEC_ORIGIN_API;
		goto out_free_temp_refs;
	}
	/*调用iotctl函数，穿透到tee驱动中，调用tee驱动的ioctl中的TEE_IOC_INVOKE操作*/
	rc = ioctl(session->ctx->fd, TEE_IOC_INVOKE, &buf_data);
	if (rc) {
		EMSG("TEE_IOC_INVOKE failed");
		eorig = TEEC_ORIGIN_COMMS;
		res = ioctl_errno_to_res(errno);
		goto out_free_temp_refs;
	}
 
	res = arg->ret;
	eorig = arg->ret_origin;
	/*解析从TA中返回到params缓存中的数据*/
	teec_post_process_operation(operation, params, shm);

out_free_temp_refs:
	teec_free_temp_refs(operation, shm);
out:
	if (error_origin)
		*error_origin = eorig;
	return res;	
}
```

# 6.TEEC_RequestCancellation

* `函数原型`：`void TEEC_RequestCancellation(TEEC_Operation *operation)`
* `函数作用描述`:取消某个`CA`与`TA`之间的操作，**该接口只能由除执行TEEC_OpenSession和TEEC_InvokeCommand的thread之外的其他thread进行调用**，而在TA端或者TEE OS可以选着并不响应该请求。只有当operation中的`started`域被设置成0之后，该操作方可有效。
* 参数说明：
	* `operation`: 已经初始化的TEEC_Operation类型的变量，该变量中包含CA于TA之间进行交互的buffer,缓存的属性等信息
* 函数实现（在OP-TEE中的实现）如下：

```cpp
void TEEC_RequestCancellation(TEEC_Operation *operation){
	struct tee_ioctl_cancel_arg arg;
	TEEC_Session *session;
	if(!operation)
		return;
	/*获取session*/
	teec_mutex_lock(&teec_mutex);
	session = operation->session;
	teec_mutex_unlock(&teec_mutex);
	
	if(!session){
		return;
	}
	arg.session = session->session_id;
	arg.cancel_id = 0;
	/*调用tee驱动中的ioctl执行TEE_IOC_CANCEL操作*/
	if(ioctl(session->ctx->fd, TEE_IOC_CANCEL, &arg)){
		EMSG("TEE_IOC_CANCEL: %s", strerror(errno));
	}
}
```

# 7.TEEC_RegisterShareMemory

* `函数原型`：`TEEC_Result TEEC_RegisterSharedMemory(TEEC_Context *ctx, TEEC_SharedMemory *shm)`
* `函数作用描述`:注册一块在CA端的内存作为CA与TA之间的共享内存。shareMemory结构体中的三个成员分别为：
	* `buffer`: 指向作为共享内存的起始地址
	* `size`: 共享内存的大小
	* `flags`: 表示CA与TA之间的数据流方向
* 参数说明：
	* `ctx`: 指向一个类型为TEEC_Context的变量，该变量必须已经被初始化了
	* `shm`:指向共享内存的结构体变量

函数实现（在OP-TEE中的实现）如下：

```cpp
TEEC_Result TEEC_RegisterSharedMemory(TEEC_Context *ctx,TEEC_SharedMemroy *shm){
	int fd;
	size_t s;
	if(!ctx || !shm){
		return TEEC_ERROR_BAD_PARAMETERS;
	}
	if(!shm->flags || (shm->flags & ~(TEEC_MEM_INPUT | TEEC_MEM_OUT))){
		return TEEC_ERROR_BAD_PARAMETERS;
	}
	s = shm->size;
	if(!s)
		s=8;
	/*调用ioctl函数穿透到tee驱动中的ioctl函数，执行TEE_IOC_SHM_ALLOC操作*/
	fd = teec_shm_alloc(ctx->fd, s, &shm->id);
	if (fd < 0)
		return TEEC_ERROR_OUT_OF_MEMORY;
	/*将注册到OP-TEE中的share memory的对应fd映射到系统内存中，并存放到shm中的shadow_buffer变量中*/
	shm->shadow_buffer = mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_SHARED,
				  fd, 0);
	close(fd);
	if (shm->shadow_buffer == (void *)MAP_FAILED) {
		shm->id = -1;
		return TEEC_ERROR_OUT_OF_MEMORY;
	}
	shm->alloced_size = s;
	shm->registered_fd = -1;
	return TEEC_SUCCESS;
}
```

# 8. TEEC_RegisterShareMemoryFileDescriptor

* `函数原型`：`TEEC_Result TEEC_RegisterSharedMemoryFileDescriptor(TEEC_Context *ctx,TEEC_SharedMemory *shm,int fd)`
* `函数作用描述`:注册一块在CA与TA之间的共享文件，在CA端会将文件的描述符fd传递給OP-TEE，其内容被存放到shm中。
* `参数说明`：
	* `ctx`: 指向一个类型为TEEC_Context的变量，该变量必须已经被初始化了。
	* `shm`:指向共享内存的结构体变量
	* `fd`:共享的文件的描述符号
* 函数实现（在OP-TEE中的实现）如下：

```cpp

TEEC_Result TEEC_RegisterSharedMemoryFileDescriptor(TEEC_Context *ctx,
						    TEEC_SharedMemory *shm,
						    int fd)
{
	struct tee_ioctl_shm_register_fd_data data;
	int rfd;
 
	if (!ctx || !shm || fd < 0)
		return TEEC_ERROR_BAD_PARAMETERS;
 
	if (!shm->flags || (shm->flags & ~(TEEC_MEM_INPUT | TEEC_MEM_OUTPUT)))
		return TEEC_ERROR_BAD_PARAMETERS;
 
/* 组合共享文件的结构体 */
	memset(&data, 0, sizeof(data));
	data.fd = fd;
 
/* 调用ioctl函数由tee驱动来完成共享文件注册的其他操作 */
	rfd = ioctl(ctx->fd, TEE_IOC_SHM_REGISTER_FD, &data);
	if (rfd < 0)
		return TEEC_ERROR_BAD_PARAMETERS;
 
/* 将返回值保存到shm变量中，以便后续被使用 */
	shm->buffer = NULL;
	shm->shadow_buffer = NULL;
	shm->registered_fd = rfd;
	shm->id = data.id;
	shm->size = data.size;
	return TEEC_SUCCESS;
}
```

# 9.TEEC_AllocateSharedMemory

* `函数原型`：`TEEC_Result TEEC_AllocateSharedMemory(TEEC_Context *ctx, TEEC_SharedMemory *shm)`
* `函数作用描述`:　分配一块共享内存，共享内存是由OP-TEE中分配的,OP-TEE分配了共享内存之后将会返回该内存块的fd给CA，CA将会将fd映射到系统内存，然后将地址保存到shm中
* `参数说明`：
	* `ctx`: 指向一个类型为TEEC_Context的变量，该变量必须已经被初始化了。
	* `shm`:指向共享内存的结构体变量
	
函数实现（在OP-TEE中的实现）如下：

```cpp
TEEC_Result TEEC_AllocateSharedMemory(TEEC_Context *ctx, TEEC_SharedMemory *shm)
{
	int fd;
	size_t s;
 
	if (!ctx || !shm)
		return TEEC_ERROR_BAD_PARAMETERS;
 
	if (!shm->flags || (shm->flags & ~(TEEC_MEM_INPUT | TEEC_MEM_OUTPUT)))
		return TEEC_ERROR_BAD_PARAMETERS;
 
	s = shm->size;
	if (!s)
		s = 8;
 
/* 通知OP-TEE进行共享内存的分配，返回fd */
	fd = teec_shm_alloc(ctx->fd, s, &shm->id);
	if (fd < 0)
		return TEEC_ERROR_OUT_OF_MEMORY;
 
/* 将fd映射进系统内存，并将映射完成的地址存放到shm中 */
	shm->buffer = mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (shm->buffer == (void *)MAP_FAILED) {
		shm->id = -1;
		return TEEC_ERROR_OUT_OF_MEMORY;
	}
	shm->shadow_buffer = NULL;
	shm->alloced_size = s;
	shm->registered_fd = -1;
	return TEEC_SUCCESS;
}
```

# 10.TEEC_ReleaseSharedMemory

* `函数原型`：`void TEEC_ReleaseSharedMemory(TEEC_SharedMemory *shm)`
* `函数作用描述`:释放已经被分配或者是注册过的共享内存
* `参数说明`：
	* `shm`:指向共享内存的结构体变量
	
函数实现（在OP-TEE中的实现）如下：

```cpp

void TEEC_ReleaseSharedMemory(TEEC_SharedMemory *shm)
{
	if (!shm || shm->id == -1)
		return;
 
/* unmap掉存放在shm中的系统内存 */
	if (shm->shadow_buffer)
		munmap(shm->shadow_buffer, shm->alloced_size);
	else if (shm->buffer)
		munmap(shm->buffer, shm->alloced_size);
	else if (shm->registered_fd >= 0)
		close(shm->registered_fd);
 
/* 清空掉shm中的成员 */
	shm->id = -1;
	shm->shadow_buffer = NULL;
	shm->buffer = NULL;
	shm->registered_fd = -1;
}
```