/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               secStorTaHandle.h
 *
 * Filename      : secStorTaHandle.h
 * Programmer(s) : system BSP
 * Filename      : secStorTaHandle.h
 * Author        : Shuai Fengyun
 * Mail          : shuaifengyun@126.cn
 * Create Time   : Mon 19 Jun 2017 10:35:10 AM CST
 ****************************************************************************************
 */

#ifndef MOUDLE_SST_HANDLE_TA_H_
#define MOUDLE_SST_HANDLE_TA_H_




/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "secStorTaType.h"





/*
 *******************************************************************************
 *                  MACRO DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/
/* Define the UUID of this TA */
#define TA_SEC_STORAGE_TESE_UUID    {0x59e4d3d3, 0x0199, 0x4f74, \
                                                { \
                                                    0xb9, 0x4d, 0x53, 0xd3, 0xda, 0xa5, 0x7d, 0x73 \
                                                } \
                                            }

    
    
/* Define the command index in this TA(Get Aes boot key) */
#define CMD_CREATE_OPER            1U     /**< Command ID of read operation in secure storage */
#define CMD_READ_OPER              2U     /**< Command ID of read operation in secure storage */
#define CMD_WRITE_OPER             3U     /**< Command ID of write operation in secure storage */
#define CMD_TRUNCATE_OPER          4U     /**< Command ID of truncate operation in secure storage */
#define CMD_RENAME_OPER            5U     /**< Command ID of rename operation in secure storage */
#define CMD_DEL_OPER               6U     /**< Command ID of close and delete operation in secure storage */
#define CMD_TEST                   7U     /**< Command ID of test operation in secure storage */


#define TEE_OBJECT_STORAGE_PRIVATE  0x00000001

 


#define OPEN_ACCESS_FLAG    TEE_DATA_FLAG_ACCESS_WRITE | \
                            TEE_DATA_FLAG_ACCESS_WRITE_META | \
                            TEE_DATA_FLAG_ACCESS_READ | \
                            TEE_DATA_FLAG_EXCLUSIVE 

#define SEEK_SET        0U
#define SEEK_CURENT     1U
#define SEEK_END        3U

/*
 *******************************************************************************
 *                STRUCTRUE DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/











#ifndef MOUDLE_SST_HANDLE_TA_C_


/*
 *******************************************************************************
 *                      VARIABLES SUPPLIED BY THIS MODULE
 *******************************************************************************
*/





/*
 *******************************************************************************
 *                      FUNCTIONS SUPPLIED BY THIS MODULE
 *******************************************************************************
*/

extern TEE_Result g_SecStorTa_CreateFile(uint32_t paramTypes, TEE_Param params[4]);
extern TEE_Result g_SecStorTa_Read(uint32_t paramTypes, TEE_Param params[4]);
extern TEE_Result g_SecStorTa_Write(uint32_t paramTypes, TEE_Param params[4]);
extern TEE_Result g_SecStorTa_Truncate(uint32_t paramTypes, TEE_Param params[4]);
extern TEE_Result g_SecStorTa_Rename(uint32_t paramTypes, TEE_Param params[4]);
extern TEE_Result g_SecStorTa_Dle(uint32_t paramTypes, TEE_Param params[4]);
extern TEE_Result store_data_sample(void);







#endif

#endif  /* MOUDLE_NAME_H*/
