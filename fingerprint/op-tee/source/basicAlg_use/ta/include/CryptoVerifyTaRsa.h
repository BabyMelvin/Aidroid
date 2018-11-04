/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyTaRsa.h
 *
 * Filename      : CryptoVerifyTaRsa.h
 * Author        : Shuai Fengyun
 * Mail          : shuai.fengyun@126.com
 * Create Time   : Fri 30 Oct 2015 01:06:17 AM EDT
 ****************************************************************************************
 */

#ifndef MOUDLE_CRYPTO_TA_RSA_H_
#define MOUDLE_CRYPTO_TA_RSA_H_




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
#define SIZE_OF_KEY_RSA1024    128U
#define SIZE_OF_KEY_RSA2048    256U



#define EM_OP_ENC     0x01
#define EM_OP_DEC     0x02
#define EM_OP_SIGN    0x03
#define EM_OP_VERIFY  0x04
#define EM_OP_ENC2048     0x05
#define EM_OP_DEC2048     0x06
#define EM_OP_SIGN2048    0x07
#define EM_OP_VERIFY2048  0x08

#define PADDING_NO    0x01
#define PADDING_PKCS1 0X02


#define PUBLIC_ATTR_NUM     0x02
#define PRIVATE_ATTR_NUM    0x03


#define VERIFY_RESULT_SUCCESS   "Verify OK!"
#define VERIFY_RESULT_FAIL      "Verify FAIL!"


/*
 *******************************************************************************
 *                STRUCTRUE DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/
typedef struct _RsaKey
{
    CHAR* Ndata;
    CHAR* Ddata;
    CHAR* Edata;
    UINT32 Nlen;
    UINT32 Dlen;
    UINT32 Elen;
    TEE_CRYPTO_ALGORITHM_ID padding;
}RsaKey;

/* RSA operation type */
typedef enum
{
    EN_OP_RSA_ENCRYPT = 1,
    EN_OP_RSA_DECRYPT,
    EN_OP_RSA_SIGN,
    EN_OP_RSA_VERIFY,
    EN_OP_RSA_INVALID
}EN_RSA_OPERATION_ACTION;


/* RSA padding type */
typedef enum
{
    EN_PADDING_PKCS1 = 1,
    EN_PADDING_PKCS7,
    EN_PADDING_NO,
    EN_PADDING_INVALID
}EN_RSA_PADDING_TYPE;


typedef enum
{
    EN_KEY_1024 = 1,
    EN_KEY_2048,
    EN_KEY_INVALID
}EN_RSA_KEY_TYPE;



#ifndef MOUDLE_CRYPTO_TA_RSA_C_


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


extern int g_CryptoTaRsa_rsaEncOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);
extern int g_CryptoTaRsa_rsaDecOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);
extern int g_CryptoTaRsa_rsaSignOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);
extern int g_CryptoTaRsa_rsaVerifyOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);















#endif

#endif  /* MOUDLE_NAME_H*/
