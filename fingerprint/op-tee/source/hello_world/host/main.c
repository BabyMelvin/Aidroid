#include <err.h>
#include <stdio.h>
#include <string.h>

//TODO OP-TEE client API (build by optee_client)
#include <tee_client_api.h>

//TODO to the the UUID (found the TA's h-file(s))
#include <hello_world_ta.H>

int main(int argc,char* argv[]){
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session  sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_HELLO_WORLD_UUID;
	uint32_t err_orign;
	
	//TODO 初始化一个context 来连接到TEE
	res = TEEC_InitializeContext(NULL,&ctx);
	if(res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x",res);
	
	//TODO 向"hello world" TA,打开一个session
	//     TA将会在session创建时候，输出"hello world!"
	res=TEEC_OpenSession(&ctx,&sess,&uuid,TEEC_LOGIN_PUBLIC,NULL,NULL,&err_orign);
	
	//TODO 声明一个 TEEC_Operation 结构体
	memset(&op,0,sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT,TEEC_NONE,TEEC_NONE,TEEC_NONE);
	op.params[0].value.a = 42;
	
	res = TEEC_InvokeCommand(&sess,TA_HELLO_WORLD_CMD_INC_VALUE,&op,&err_origin);
	
	printf("TA incremented value to %d \n",op.params[0].value.a);
	
	//TODO 处理完成，关闭session和销毁context
	// TA 当session关闭时候，将会打印"Goodbye"
	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);
	return 0;
}
