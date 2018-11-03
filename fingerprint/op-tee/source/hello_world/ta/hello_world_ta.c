#define STR_TRACE_USER_TA "HELLO_WORLD"

#include <tee_internal_api.h>
#include <tee_internal_api_extension.h>

#include "hello_world.ta.h"

//TODO 当TA被创建时候调用。这是TA第一个调用
TEE_Result TA_CreateEntryPoint(void){
	DMSG("has been called");
	return TEE_SUCCESS;
}

//TODO 当TA实例被销毁(如果这个TA没有crashed或者panicked)
//     在TA中最后被调用
void TA_DestroyEntryPoint(void){
	DMSG("has been called");
}

//TODO 当对TA打开一个session时候调用。
//	   *sess_ctx将会记录一个session值，用来接下来的调用
//     这个函数通常用来多一些TA的全局初始化工作
TEE_Result TA_OpenSeesionEntryPoint(uint32_t param_types,
			TEE_Param __maybe_unused params[4],
			void __maybe_unused **sessctx){
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
							TEE_PARAM_TYPE_NONE,
							TEE_PARAM_TYPE_NONE,
							TEE_PARAM_TYPE_NONE);
	if(param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;
	//unused parameters
	(void)&params;
	(void)&sess_ctx;
	DMSG("Hello World\n");
	
	//if return value!= TEE_SUCCESS  the session will not be created
	return TEE_SUCCESS;
}
//TODO session关闭时候调用，sess_ctx保持着TA_OpenSessionEntryPonit()的值
void TA_CloseSessionEntryPonit(void __maybe_unused *sess_ctx){
	(void)&sess_ctx;//unused parameters
	DMSG("Goodbye!\n");
}
//TODO TA被invoke时候调用.sess_ctx保持着TA_OpenSessionEntryPonit的值。
// 其他参数来自normal world
TEE_Result TA_InvokeCommandEntryPonit(void __maybe_unused*sess_ctx,
		uint32_t cmd_id,
		uint32_t param_types,TEE_Param params[4]){
	(void)&sess_ctx;//unused parameters
	switch(cmd_id){
		case TA_HELLO_WORLD_CMD_INC_VALUE:
			return inc_value(param_types,params);
		default:
			return TEE_ERROR_BAD_PARAMETERS;
	}
}

static TEE_Result inc_value(uint32_t param_types,
				TEE_Param params[4]){
	uint32_t exp_param_types=TEE_PARAM_TYPES(TEE_PARAM_VALUE_INOUT,
						TEE_PARAM_VALUE_NONE,
						TEE_PARAM_VALUE_NONE,
						TEE_PARAM_VALUE_NONE);
	DMSG("has been called");
	if(param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;
	
	DMSG("Got value:%u from NW",params[0].value.a);
	params[0].value.a++;
	DMSG("Increase value to :%u",params[0].value.a);
	return TEE_SUCCESS;
}




















