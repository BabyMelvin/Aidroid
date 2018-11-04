/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyTaHash.c
 *
 * Filename      : CryptoVerifyTaHash.c
 * Author        : Shuai Fengyun
 * Mail          : shuai.fengyun@126.com
 * Create Time   : Fri 30 Oct 2015 12:40:28 AM EDT
 ****************************************************************************************
 */

#define MOUDLE_CRYPTO_TA_HASH_C_

/** @defgroup MODULE_NAME_INFOR
* @{
*/

/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "CryptoVerifyTaHash.h"
#include "CryptoVerifyTaDebug.h"




/*
 *******************************************************************************
 *                         FUNCTIONS SUPPLIED BY THIS MODULE
 *******************************************************************************
*/





/*
 *******************************************************************************
 *                          VARIABLES SUPPLIED BY THIS MODULE
 *******************************************************************************
*/





/*
 *******************************************************************************
 *                          FUNCTIONS USED ONLY BY THIS MODULE
 *******************************************************************************
*/
int g_CryptoTaHash_sha(EN_SHA_MODE shaMode, CHAR* input, UINT32 inLen, CHAR* output, UINT32* pOutLen);
int g_CryptoTaHash_hmac(CHAR* output, UINT32 OutLen, CHAR* passWd, UINT32 passLen, CHAR* salt, UINT32 saltLen, UINT32 count);






/*
 *******************************************************************************
 *                          VARIABLES USED ONLY BY THIS MODULE
 *******************************************************************************
*/





/*
 *******************************************************************************
 *                               FUNCTIONS IMPLEMENT
 *******************************************************************************
*/
/** @ingroup MOUDLE_NAME_C_
 *- #Description  This function for handle command.
 * @param   pMsg           [IN] The received request message
 *                               - Type: MBX_Msg *
 *                               - Range: N/A.
 *
 * @return     void
 * @retval     void
 *
 *
 */
int g_CryptoTaHash_sha(EN_SHA_MODE shaMode, CHAR* input, UINT32 inLen, CHAR* output, UINT32* pOutLen)
{
    TEE_Result ret;
    TEE_OperationHandle l_OperationHandle;   
    TEE_CRYPTO_ALGORITHM_ID l_AlgorithmId;
    int l_RetVal = OK;
    
    TF("Input data just like follow(0x%x), 0x%x:\n", inLen, (UINT32)(output));
    //g_TA_Printf(input, 20);

    /**1) Set the algorithm variable */
    switch(shaMode)
    {
        case EN_OP_SHA1:
            TF("The md is sha1@!!!!\n");
            l_AlgorithmId = TEE_ALG_SHA1;
            break;
        case EN_OP_SHA256:
            l_AlgorithmId = TEE_ALG_SHA256;
            break;
        default:
            l_AlgorithmId = TEE_ALG_INVALID ;
            l_RetVal = FAIL;
            goto cleanup_1;
            TF("Invalid sha mode\n");
            break;
    }

    /**2) Allocate the operation handle */
    ret = TEE_AllocateOperation(&l_OperationHandle, l_AlgorithmId, TEE_MODE_DIGEST, 0);
    if(ret != TEE_SUCCESS) 
    {
        TF("Allocate SHA operation handle fail\n");
        l_RetVal = FAIL;
        goto cleanup_1;
    }

    TEE_DigestUpdate(l_OperationHandle, input, inLen);

    /**4) Do the final sha operation */
    ret = TEE_DigestDoFinal(l_OperationHandle, NULL, 0, output, pOutLen);
    TF("The out put length is :%d\n", *pOutLen);
    TF("The return value is :0x%x\n", ret);
    //ret = TEE_DigestDoFinal(l_OperationHandle, l_pInputTmp, inLen, output, pOutLen);
    if(ret != TEE_SUCCESS)
    {
        TF("Do the final sha operation fail\n");
        l_RetVal = FAIL;
        goto cleanup_2;
    }

    TF("Hash value just like folloe:\n");
    g_TA_Printf(output, *pOutLen);

    /**5) Do the clean up operation& return the result */
    cleanup_2:
        TEE_FreeOperation(l_OperationHandle);
    cleanup_1:
        return l_RetVal;
}






/** @ingroup MOUDLE_NAME_C_
 *- #Description  This function for handle command.
 * @param   pMsg           [IN] The received request message
 *                               - Type: MBX_Msg *
 *                               - Range: N/A.
 *
 * @return     void
 * @retval     void
 *
 *
 */
int g_CryptoTaHash_hmac(CHAR* output, UINT32 OutLen, CHAR* passWd, UINT32 passLen, CHAR* salt, UINT32 saltLen, UINT32 count)
{
    TEE_Result ret;
    int l_RetVal = OK;
    TEE_OperationHandle l_OperationHandle;
    TEE_ObjectHandle l_SvHandle;
    TEE_ObjectHandle l_PassWdObjHandle;
    TEE_Attribute l_Attr;
    TEE_Attribute l_Param[3] = {0};

    
    TF("Pwd:%s\n", passWd);
    TF("Pwd len:%d\n", passLen);
    TF("Salt:%s\n", salt);
    TF("Salt len:%d\n", saltLen);
    TF("dkLen:%d\n", OutLen);
    TF("C:%d\n", count);

    /**2) Allocate the operation handle */
    ret = TEE_AllocateOperation(&l_OperationHandle, TEE_ALG_PBKDF2_HMAC_SHA1_DERIVE_KEY, TEE_MODE_DERIVE, 2048);
    if(ret != TEE_SUCCESS) 
    {
        TF("Allocate SHA operation handle fail\n");
        l_RetVal = FAIL;
        goto cleanup_1;
    }

    /**4) Do the final sha operation */
    ret = TEE_AllocateTransientObject(TEE_TYPE_PBKDF2_PASSWORD, 2048, &l_PassWdObjHandle);
    //TF("The out put length is :%d\n", *pOutLen);
    if(ret != TEE_SUCCESS)
    {
        TF("Do the final hmac operation fail\n");
        l_RetVal = FAIL;
        goto cleanup_2;
    }

    l_Attr.attributeID = TEE_ATTR_PBKDF2_PASSWORD;
    l_Attr.content.ref.buffer = passWd;
    l_Attr.content.ref.length = passLen;
    
    /**4) Do the final sha operation */
    ret = TEE_PopulateTransientObject(l_PassWdObjHandle, &l_Attr, 1);
    //TF("The out put length is :%d\n", *pOutLen);
    if(ret != TEE_SUCCESS)
    {
        TF("Do the final hmac operation fail\n");
        l_RetVal = FAIL;
        goto cleanup_2;
    }

    ret = TEE_SetOperationKey(l_OperationHandle, l_PassWdObjHandle);
    if(ret != TEE_SUCCESS)
    {
        TF("Do the final hmac operation fail\n");
        l_RetVal = FAIL;
        goto cleanup_2;
    }
    
    /**4) Do the final sha operation */
    ret = TEE_AllocateTransientObject(TEE_TYPE_GENERIC_SECRET, OutLen*8, &l_SvHandle);
    //TF("The out put length is :%d\n", *pOutLen);
    if(ret != TEE_SUCCESS)
    {
        TF("Do the final hmac operation fail\n");
        l_RetVal = FAIL;
        goto cleanup_2;
    }

    l_Param[0].attributeID = TEE_ATTR_PBKDF2_SALT;
    l_Param[0].content.ref.buffer = salt;
    l_Param[0].content.ref.length = saltLen;

    l_Param[1].attributeID = TEE_ATTR_PBKDF2_DKM_LENGTH;
    l_Param[1].content.value.a = OutLen;
    l_Param[1].content.value.b = 0;
    
    l_Param[2].attributeID = TEE_ATTR_PBKDF2_ITERATION_COUNT;
    l_Param[2].content.value.a = count;
    l_Param[2].content.value.b = 0;

    TEE_DeriveKey(l_OperationHandle, l_Param, 3, l_SvHandle);

    ret = TEE_GetObjectBufferAttribute(l_SvHandle, TEE_ATTR_SECRET_VALUE, output, &OutLen);
    //TF("The out put length is :%d\n", *pOutLen);
    if(ret != TEE_SUCCESS)
    {
        TF("Do the final hmac operation fail\n");
        l_RetVal = FAIL;
        goto cleanup_2;
    }

    //TF("Hash value just like folloe:\n");
    g_TA_Printf(output, OutLen);

    /**5) Do the clean up operation& return the result */
    cleanup_2:
        TEE_FreeOperation(l_OperationHandle);
    cleanup_1:
        return l_RetVal;
}








/**
 * @}
 */
