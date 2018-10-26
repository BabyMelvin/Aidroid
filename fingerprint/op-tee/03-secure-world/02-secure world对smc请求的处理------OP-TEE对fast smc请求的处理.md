在OP-TEE中将所有的smc请求分为fast smc和std smc，monitor模式中通过判定从REE侧传递的第一个参数的bit31是否为1来判定该请求是fast smc还是std smc。

`fast smc`一般会在驱动挂载过程中或者需要获取OP-TEE OS的版本信息，共享内存配置，cache信息的时候被调用。**fast smc的特点就是在OP-TEE不会使用一个thread来对fast smc进行处理**，**而是在OP-TEE的kernel space层面直接对smc进行请求，并返回处理结果**。
`《29. secure world对smc请求的处理------monitor模式中的处理》`一文中介绍了smc请求在monitor模式中的处理过程，在此过程中会在sm_from_nsec函数中对smc的类型进行判定。当判定结果为fast smc时会调用`thread_vector_table.fast_smc_entry`进行fast smc请求的处理。

在sm_from_nsec中调用最终会调用到thread_vector_table向量表中的vector_fast_smc_entry函数来对fast smc进行处理。该函数使用汇编实现，定义在`optee_os/core/arch/arm/kernel/thread_a32.S`中，其内容如下：

```asm
LOCAL_FUNC vector_fast_smc_entry , :
UNWIND(	.fnstart)
UNWIND(	.cantunwind)
	push	{r0-r7}	//将r0~r7入栈
	mov	r0, sp	//将栈地址赋值给r0
	bl	thread_handle_fast_smc	//调用thread_handle_fast_smc进行处理，参数为r0中的数据
	pop	{r1-r8}	//处理完成之后，执行出栈操作
	ldr	r0, =TEESMC_OPTEED_RETURN_CALL_DONE 	//r0存放fast smc处理操作的结果
	smc	#0	//触发smc请求 切换到monitor模式，返回normal world中
	b	.	/* SMC should not return */
UNWIND(	.fnend)
END_FUNC vector_fast_smc_entry
 
void thread_handle_fast_smc(struct thread_smc_args *args)
{
/* 使用canaries原理检查栈空间是否存在溢出或者被破坏 */
	thread_check_canaries();
 
/* 调用thread_fast_smc_handler_ptr处理smc请求 */
	thread_fast_smc_handler_ptr(args);
	/* Fast handlers must not unmask any exceptions */
	assert(thread_get_exceptions() == THREAD_EXCP_ALL);
}

```

在OP-TEE启动的时候会执行init_handlers操作，该函数的主要作用是将真正的处理函数赋值给各种thread函数指针变量。关于init_handlers函数的调用和处理过程请查阅前期文章。thread_fast_smc_handler_ptr会被赋值成`handlers->fast_smc`，而在vxpress板级中`handlers->fast_smc`执行tee_entry_fast函数。该函数内容如下:

```cpp
void tee_entry_fast(struct thread_smc_args *args)
{
	switch (args->a0) {
 
	/* Generic functions */
/* 获取API被调用的次数，可以根据实际需求实现 */
	case OPTEE_SMC_CALLS_COUNT:
		tee_entry_get_api_call_count(args);
		break;
/* 获取OP-TEE API的UID值 */
	case OPTEE_SMC_CALLS_UID:
		tee_entry_get_api_uuid(args);
		break;
/* 获取OP-TEE中API的版本信息 */
	case OPTEE_SMC_CALLS_REVISION:
		tee_entry_get_api_revision(args);
		break;
/* 获取OP-TEE OS的UID值 */
	case OPTEE_SMC_CALL_GET_OS_UUID:
		tee_entry_get_os_uuid(args);
		break;
/* 获取OS的版本信息 */
	case OPTEE_SMC_CALL_GET_OS_REVISION:
		tee_entry_get_os_revision(args);
		break;
 
	/* OP-TEE specific SMC functions */
/* 获取OP-TEE与驱动之间的共享内存配置信息 */
	case OPTEE_SMC_GET_SHM_CONFIG:
		tee_entry_get_shm_config(args);
		break;
/* 获取I2CC的互斥体信息 */
	case OPTEE_SMC_L2CC_MUTEX:
		tee_entry_fastcall_l2cc_mutex(args);
		break;
/* OP-TEE的capabilities信息 */
	case OPTEE_SMC_EXCHANGE_CAPABILITIES:
		tee_entry_exchange_capabilities(args);
		break;
/* 关闭OP-TEE与驱动的共享内存的cache */
	case OPTEE_SMC_DISABLE_SHM_CACHE:
		tee_entry_disable_shm_cache(args);
		break;
/* 使能OP-TEE与驱动之间共享内存的cache */
	case OPTEE_SMC_ENABLE_SHM_CACHE:
		tee_entry_enable_shm_cache(args);
		break;
/* 启动其他cortex的被使用 */
	case OPTEE_SMC_BOOT_SECONDARY:
		tee_entry_boot_secondary(args);
		break;
 
	default:
		args->a0 = OPTEE_SMC_RETURN_UNKNOWN_FUNCTION;
		break;
	}
}
```
从上面的函数可以看到，tee_entry_fast会根据不同的command ID来执行特定的操作。使用者也可以在此函数中添加自己需要fast smc实现的功能，只要在REE侧和OP-TEE中定义合法fast smc的command ID并实现具体操作就可以

