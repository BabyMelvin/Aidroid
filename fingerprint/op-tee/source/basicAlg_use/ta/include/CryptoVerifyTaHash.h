/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyTaHash.h
 *
 * Filename      : CryptoVerifyTaHash.h
 * Author        : Shuai Fengyun
 * Mail          : shuai.fengyun@126.com
 * Create Time   : Fri 30 Oct 2015 01:05:38 AM EDT
 ****************************************************************************************
 */

#ifndef MOUDLE_CRYPTO_TA_HASH_H_
#define MOUDLE_CRYPTO_TA_HASH_H_




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





/*
 *******************************************************************************
 *                STRUCTRUE DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/
/* SHA operation type */
typedef enum
{
    EN_OP_SHA1 = 1,
    EN_OP_SHA224,
    EN_OP_SHA256,
    EN_OP_SHA384,
    EN_OP_SHA512,
    EN_OP_SHA_INVALID
}EN_SHA_MODE;





#ifndef MOUDLE_CRYPTO_TA_HASH_C_


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


extern void test_hash(void);

extern int g_CryptoTaHash_sha(EN_SHA_MODE shaMode, CHAR* input, UINT32 inLen, CHAR* output, UINT32* pOutLen);
extern int g_CryptoTaHash_hmac(CHAR* output, UINT32 OutLen, CHAR* passWd, UINT32 passLen, CHAR* salt, UINT32 saltLen, UINT32 count);















#endif

#endif  /* MOUDLE_NAME_H*/
