/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               secStorCaHandle.h
 *
 * Filename      : secStorCaHandle.h
 * Author        : Shuai Fengyun
 * Mail          : shuaifengyun@126.com
 * Create Time   : Mon 19 Jun 2017 03:50:39 PM CST
 ****************************************************************************************
 */

#ifndef MOUDLE_SST_HANDLE_CA_H_
#define MOUDLE_SST_HANDLE_CA_H_




/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/





/*
 *******************************************************************************
 *                  MACRO DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/
/* Define the command index in this TA(Get Aes boot key) */
#define CMD_CREATE_OPER            1U     /**< Command ID of read operation in secure storage */
#define CMD_READ_OPER              2U     /**< Command ID of read operation in secure storage */
#define CMD_WRITE_OPER             3U     /**< Command ID of write operation in secure storage */
#define CMD_TRUNCATE_OPER          4U     /**< Command ID of truncate operation in secure storage */
#define CMD_RENAME_OPER            5U     /**< Command ID of rename operation in secure storage */
#define CMD_DEL_OPER               6U     /**< Command ID of close and delete operation in secure storage */
#define CMD_TEST                   7U     /**< Command ID of test operation in secure storage */

/* Define the UUID of this TA */
#define TA_SEC_STORAGE_TESE_UUID    {0x59e4d3d3, 0x0199, 0x4f74, \
                                                { \
                                                    0xb9, 0x4d, 0x53, 0xd3, 0xda, 0xa5, 0x7d, 0x73 \
                                                } \
                                            }




/*
 *******************************************************************************
 *                STRUCTRUE DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/


#ifndef MOUDLE_SST_HANDLE_CA_C_


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
extern int g_SecStorCa_CreateFile(UINT32 len, CHAR* fileName);
extern int g_SecStorCa_ReadFile(UINT32 fLen, CHAR* fName , UINT32 len, CHAR* output);
extern int g_SecStorCa_WiteFile(UINT32 fLen, CHAR* fName , UINT32 len, CHAR* input);
extern int g_SecStorCa_TrunCatFile(UINT32 fLen, CHAR* fName, UINT32 size);
extern int g_SecStorCa_RenameFile(UINT32 oldLen, CHAR* oldName, UINT32 newLen, CHAR* newName);
extern int g_SecStorCa_DeleteFile(UINT32 oldLen, CHAR* oldName);
extern void g_SecStorCa_test();





















#endif

#endif  /* MOUDLE_NAME_H*/
