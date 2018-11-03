#ifndef USER_TA_HEADER_DEFINES_H
#define USER_TA_HEADER_DEFINES_H

//TODO 获取 TA_HELLO_WORLD_UUID 定义
#include <hello_world_ta.h> 
#define TA_UUID TA_HELLO_WORLD_UUID

#define TA_FLAGS  			(TA_FLAGS_MULTI_SESSION | TA_FLAGS_EXEC_DDR)
#define TA_STACK_SIZE		(2 * 1024)
#define TA_DATA_SIZE		(32 * 1024）

#define TA_CURRENT_TA_EXE_PROPERTIES \
	{"gp.ta.description",USER_TA_PROP_TYPE_STRING,"hello world TA"} \
	{"gp.ta.version",USER_TA_PROP_TYPE_U32,&(const uint32_t){0x0010} \

#endif