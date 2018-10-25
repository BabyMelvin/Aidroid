
/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyTaAes.h
 *
 * Filename      : CryptoVerifyTaAes.h
 * Author        : Shuai Fengyun
 * Mail          : shuai.fengyun@126.com
 * Create Time   : Fri 30 Oct 2015 01:04:45 AM EDT
 ****************************************************************************************
 */

#ifndef MOUDLE_CRYPTO_TA_AES_H_
#define MOUDLE_CRYPTO_TA_AES_H_




/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "CryptoVerifyTaType.h"





/*
 *******************************************************************************
 *                  MACRO DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/
#define SIZE_OF_AES256_KEY      32U
#define SIZE_OF_AES128_KEY      16U

#define SIZE_OF_AES256_IV       32U
#define SIZE_OF_AES128_IV       16U

#define SIZE_OF_AES128_BLOCK_LEN 16U
#define SIZE_OF_AES256_BLOCK_LEN 32U





/*
 *******************************************************************************
 *                STRUCTRUE DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/
typedef struct _AesOperation
{
    CHAR* inBuf;
    CHAR* outBuf;
    CHAR* key;
    CHAR* iv;
    UINT32 dataLen;
    UINT32 keyLen;
    UINT32 ivLen;
    UINT32 algorithmId;
    TEE_OperationMode operMode;
}AesOperation;




/* AES operation type */
typedef enum
{
    EN_OP_AES_ENCRYPT = 1,
    EN_OP_AES_DECRYPT,
    EN_OP_AES_INVALID
}EN_AES_OPERATION_ACTION;

/* AES mode type */
typedef enum
{
    EN_MODE_CBC = 1,
    EN_MODE_ECB,
    EN_MODE_CTR,
    EN_MODE_CBC_CTS,
    EN_MODE_INVALIE
}EN_AES_MODE;


typedef struct _AesOperModeInfo
{
    EN_AES_OPERATION_ACTION active;
    EN_AES_MODE mode;
}AesOperModeInfo;



#ifndef MOUDLE_CRYPTO_TA_AES_C_


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
extern int g_CryptoTaAes_AesOper(AesOperation aesOper);


















#endif

#endif  /* MOUDLE_NAME_H*/
