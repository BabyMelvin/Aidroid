/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyTaHandle.h
 *
 * Filename      : CryptoVerifyTaHandle.h
 * Author        : Shuai Fengyun
 * Mail          : shuai.fengyun@126.com
 * Create Time   : Fri 30 Oct 2015 01:05:20 AM EDT
 ****************************************************************************************
 */

#ifndef MOUDLE_CRYPTO_TA_HANDLE_H_
#define MOUDLE_CRYPTO_TA_HANDLE_H_




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

/* Define the UUID of this TA */
#define TA_MY_CRYPTO_VERIFY_UUID    {0xebb6f4b5, 0x7e33, 0x4ad2, \
                                        { \
                                            0x98, 0x02, 0xe6, 0x4f, 0x2a, 0x7c, 0xc2, 0x0c \
                                        } \
                                    }


/* Define the command index in this TA(Get Aes boot key) */
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



/*
 *******************************************************************************
 *                STRUCTRUE DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/


#ifndef MOUDLE_CRYPTO_TA_HANDLE_C_


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




extern int g_CryptoTaHandle_Random(uint32_t paramTypes, TEE_Param params[4]);
extern int g_CryptoTaHandle_Sha(uint32_t paramTypes, TEE_Param params[4]);
extern int g_CryptoTaHandle_Aes(uint32_t paramTypes, TEE_Param params[4]);
extern int g_CryptoTaHandle_Rsa(uint32_t paramTypes, TEE_Param params[4], UINT32 opMode, UINT32 padding);
extern int g_CryptoTaHandle_Pbkdf(uint32_t paramTypes, TEE_Param params[4]);
extern int g_CryptoTaHandle_hmac(uint32_t paramTypes, TEE_Param params[4]);
extern int g_CryptoTaHandle_base64(uint32_t paramTypes, TEE_Param params[4]);











#endif

#endif  /* MOUDLE_NAME_H*/
