#ifndef MOUDLE_MY_TEST_HANDLE_H_
#define MOUDLE_MY_TEST_HANDLE_H_

#include "tee_internal_api.h"
#include "tee_api_defines.h"
#include "trace.h"
#include "tee_api_defines_extensions.h"

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


/* Define the type of variable */
typedef unsigned char  UINT8;    /**< Typedef for 8bits unsigned integer  */
typedef unsigned short UINT16;   /**< Typedef for 16bits unsigned integer */
typedef uint32_t       UINT32;   /**< Typedef for 32bits unsigned integer */
typedef signed char    INT8;     /**< Typedef for 8bits signed integer    */
typedef signed short   INT16;    /**< Typedef for 16bits signed integer   */
typedef signed int     INT32;    /**< Typedef for 32bits signed integer   */
typedef char           CHAR;     /**< Typedef for char                    */
typedef uint32_t       TEE_CRYPTO_ALGORITHM_ID;

extern int 	g_CryptoTaHandle_Sha(uint32_t paramTypes, TEE_Param params[4]);
extern int 	g_CryptoTaHandle_Random(uint32_t paramTypes, TEE_Param params[4]);
extern void g_TA_printf(CHAR* buf, UINT32 len);





















#endif  /* MOUDLE_NAME_H*/
