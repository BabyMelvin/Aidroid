
/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyCa.h
 *
 * Filename      : CryptoVerifyCa.h
 * Author        : Shuai Fengyun
 * Mail          : shuai.fengyun@126.com
 * Create Time   : Wed 11 Nov 2015 03:10:52 PM CST
 ****************************************************************************************
 */

#ifndef MOUDLE_CRYPTO_VERIFY_CA_H_
#define MOUDLE_CRYPTO_VERIFY_CA_H_




/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "tee_client_api.h"
#include <unistd.h>
#include "CryptoVerifyType.h"







/*
 *******************************************************************************
 *                  MACRO DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/
/* Defien the task IV for this CA */
//ebb6f4b5-7e33-4ad2-9802-e64f2a7cc20c

#define CRYPTO_VERIFY_UUID_ID {0xebb6f4b5, 0x7e33, 0x4ad2, \
                                  { \
                                      0x98, 0x02, 0xe6, 0x4f, 0x2a, 0x7c, 0xc2, 0x0c \
                                  } \
                              }

#define CRYPTO_VERIFY_TASK "CV_task"          /**< TA name of managing pay key    */


/* Define the comman ID */
#define CMD_GEN_RANDOM_OPER                   1U     /**< Command ID of using RSA algorithm to signa data */
#define CMD_SHA_OPER                          2U     /**< Command ID of using RSA algorithm to signa data */
#define CMD_AES_OPER                          3U
#define CMD_PBKDF_OPER                        4U
#define CMD_MAX_NUMBER                        5U     /**< The max command number in this TA           */ 
#define CMD_RSA_ENC_PKCS1_OPER            6U     /**< Command ID of using RSA algorithm to signa data */
#define CMD_RSA_DEC_PKCS1_OPER            7U
#define CMD_RSA_SIGN_PKCS1_OPER           8U
#define CMD_RSA_VERIFY_PKCS1_OPER         9U    
#define CMD_HMAC_OPER                          10U 
#define CMD_BASE64_OPER                 11U


#define CMD_BN_OPER                          14U 





/* Define the return value of function */
#define FAIL -1
#define OK   0









/*
 *******************************************************************************
 *                STRUCTRUE DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/




#ifndef MOUDLE_CRYPTO_VERIFY_CA_C_


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
extern int g_CryptoVerifyCa_Random(UINT32 len, CHAR* output);
extern int g_CryptoVerifyCa_Sha(CHAR* pData, UINT32 len, EN_SHA_MODE shaMode, CHAR* output, UINT32 outLen);
extern int g_CryptoVerifyCa_Pbkdf(UINT32 len, CHAR* output);
extern int g_CryptoVerifyCa_Rsa(RsaCaPara rsaPara);
extern int g_CryptoVerifyCa_Aes(CHAR* pData, UINT32 len, EN_AES_MODE aesMode, 
                         EN_AES_OPERATION_ACTION operAction, CHAR* output);

extern int g_CryptoVerifyCa_hmac(UINT32 len, CHAR* output, UINT32 count);
extern int g_CryptoVerifyCa_base64(CHAR* input, UINT32 lenIn, CHAR* output, UINT32 lenOut, UINT32 oper);













#endif

#endif  /* MOUDLE_NAME_H*/
