/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyTaRsa.c
 *
 * Filename      : CryptoVerifyTaRsa.c
 * Author        : Shuai Fengyun
 * Mail          : shuai.fengyun@126.com
 * Create Time   : Fri 30 Oct 2015 12:40:14 AM EDT
 ****************************************************************************************
 */

#define MOUDLE_CRYPTO_TA_RSA_C_

/** @defgroup MODULE_NAME_INFOR
* @{
*/

/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "CryptoVerifyTaRsa.h"
#include "CryptoVerifyTaDebug.h"
#include "CryptoVerifyTaHash.h"

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
int g_CryptoTaRsa_rsaEncOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);
int g_CryptoTaRsa_rsaDecOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);
int g_CryptoTaRsa_rsaSignOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);
int g_CryptoTaRsa_rsaVerifyOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);
int g_CryptoTaRsa_rsaEncOper2048(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);
int g_CryptoTaRsa_rsaDecOper2048(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);
int g_CryptoTaRsa_rsaSignOper2048(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);
int g_CryptoTaRsa_rsaVerifyOper2048(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen);





/*
 *******************************************************************************
 *                          VARIABLES USED ONLY BY THIS MODULE
 *******************************************************************************
*/
TEE_OperationHandle g_pOperationHandle = TEE_HANDLE_NULL;
UINT32 g_MaxKeySize = 0U;


TEE_ObjectHandle g_PublicKeyObjHandle = TEE_HANDLE_NULL;
TEE_Attribute g_PublicKeyAttr[2];


TEE_ObjectHandle g_KeyPairObjHandle = TEE_HANDLE_NULL;
TEE_Attribute g_PrivateKeyAttr[3];


CHAR g_RsaRaw[] = "YAO YAO qie ke nao! Zai han leng de dong tian lai tao jian bing guo zi, you!";

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
static void l_CryptoTaRsa_CleanUpKeyPairObj(void)
{
    if(TEE_HANDLE_NULL != g_KeyPairObjHandle)
    {
        TEE_FreeTransientObject(g_KeyPairObjHandle);
    }

    if(TEE_HANDLE_NULL != g_pOperationHandle)
    {
        TEE_FreeOperation(g_pOperationHandle);
    }
}


static void l_CryptoTaRsa_CleanUpPublicHandle(void)
{
    if(TEE_HANDLE_NULL != g_PublicKeyObjHandle)
    {
        TEE_FreeTransientObject(g_PublicKeyObjHandle);
    }

    if(TEE_HANDLE_NULL != g_pOperationHandle)
    {
        TEE_FreeOperation(g_pOperationHandle);
    }
}




static int l_CryptoTaRsa_SetRsaPrivateKey(RsaKey rsaOper)
{
    TEE_Result l_TeeRetVal = TEE_FAIL;
    int l_Result = OK;

    /** 1) Set the max size of key */
    g_MaxKeySize = (rsaOper.Nlen)*8U;

    /** 2) Clean attribute array */
    TEE_MemFill(g_PrivateKeyAttr, 0, 3*(sizeof(TEE_Attribute)));
    
    /** 3) Set attribute[0] data with N data */
    g_PrivateKeyAttr[0].attributeID = TEE_ATTR_RSA_MODULUS;
    g_PrivateKeyAttr[0].content.ref.buffer = rsaOper.Ndata;
    g_PrivateKeyAttr[0].content.ref.length = rsaOper.Nlen;

    /** 4) Set attribute[1] data with E data */
    g_PrivateKeyAttr[1].attributeID = TEE_ATTR_RSA_PUBLIC_EXPONENT;
    g_PrivateKeyAttr[1].content.ref.buffer = rsaOper.Edata;
    g_PrivateKeyAttr[1].content.ref.length = rsaOper.Elen; 

    /** 5) Set attribute[1] data with D data */
    g_PrivateKeyAttr[2].attributeID = TEE_ATTR_RSA_PRIVATE_EXPONENT;
    g_PrivateKeyAttr[2].content.ref.buffer = rsaOper.Ddata;
    g_PrivateKeyAttr[2].content.ref.length = rsaOper.Dlen; 


    /** 6) Allocate the public key handle */
    l_TeeRetVal = TEE_AllocateTransientObject(TEE_TYPE_RSA_KEYPAIR, g_MaxKeySize, &g_KeyPairObjHandle);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("Do allocate obj handle fail, Ret value is:0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto done;
    }


    /** 7) Populate the attribute */
    l_TeeRetVal = TEE_PopulateTransientObject(g_KeyPairObjHandle, g_PrivateKeyAttr, 3);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("Do populate obj handle fail, Ret value is:0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto cleanUp1;
    }


    /** 8) Return the operation result */
    return l_Result;

    /* Do clean up operation when have some operation faile */
cleanUp1:
    TEE_FreeTransientObject(g_KeyPairObjHandle);
    g_KeyPairObjHandle = TEE_HANDLE_NULL;
done:
    return l_Result;
}

static int l_CryptoTaRsa_SetRsaPublicKey(RsaKey rsaOper)
{
    TEE_Result l_TeeRetVal = TEE_FAIL;
    int l_Result = OK;

    /** 1) Set the max size of key */
    g_MaxKeySize = (rsaOper.Nlen)*8U;

    /** 2) Clean attribute array */
    TEE_MemFill(g_PublicKeyAttr, 0, 2*(sizeof(TEE_Attribute)));
    
    /** 3) Set attribute[0] data with N data */
    g_PublicKeyAttr[0].attributeID = TEE_ATTR_RSA_MODULUS;
    g_PublicKeyAttr[0].content.ref.buffer = rsaOper.Ndata;
    g_PublicKeyAttr[0].content.ref.length = rsaOper.Nlen;

    /** 4) Set attribute[1] data with E data */
    g_PublicKeyAttr[1].attributeID = TEE_ATTR_RSA_PUBLIC_EXPONENT;
    g_PublicKeyAttr[1].content.ref.buffer = rsaOper.Edata;
    g_PublicKeyAttr[1].content.ref.length = rsaOper.Elen; 

    /** 5) Allocate the public key handle */
    l_TeeRetVal = TEE_AllocateTransientObject(TEE_TYPE_RSA_PUBLIC_KEY, g_MaxKeySize, &g_PublicKeyObjHandle);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("Do allocate obj handle fail, Ret value is:0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto done;
    }

    /** 6) Populate the attribute */
    l_TeeRetVal = TEE_PopulateTransientObject(g_PublicKeyObjHandle, g_PublicKeyAttr, 2);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("Do populate obj handle fail, Ret value is:0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto cleanUp1;
    }

    /** 7) Return the operation result */
    return l_Result;

    /* Do clean up operation when have some operation faile */
cleanUp1:
    TEE_FreeTransientObject(g_PublicKeyObjHandle);
    g_PublicKeyObjHandle = TEE_HANDLE_NULL;
done:
    return l_Result;
}


int g_CryptoTaRsa_rsaEncOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen)
{
    TEE_Result l_TeeRetVal = TEE_FAIL;
    int l_Result = OK;

    /** 1) Set the public key object handle */
    l_Result = l_CryptoTaRsa_SetRsaPublicKey(rsaOper);
    if(FAIL == l_Result)
    {
        TF("Set public key object handle faile!\n");
        goto done;
    }

    /** 7) Allocate the operation handle */
    l_TeeRetVal = TEE_AllocateOperation(&g_pOperationHandle, rsaOper.padding, TEE_MODE_ENCRYPT, g_MaxKeySize);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("The allocate operate handle fail, the return value is: 0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto done;
    }
    
    /** 8) Assemble the key object into operation handle */
    l_TeeRetVal = TEE_SetOperationKey(g_pOperationHandle, g_PublicKeyObjHandle);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("Set operation key faile, return value is:0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto cleanUp_1;
    } 

    /** 9) Do cipher operation, judge the operation mode to do encrypto or decrypt */
    l_TeeRetVal = TEE_AsymmetricEncrypt(g_pOperationHandle, NULL, 0, inBuf, inLen, outBuf, pOutLen);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("Do asynmentric encrypy operation fail, return valus is:ox%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto cleanUp_1;
    }
    else
    {
        TF("The out put length is:%d\n", *pOutLen);
        g_TA_Printf(outBuf, *pOutLen);
        l_Result = OK;
        goto done;
    }

cleanUp_1:
    TEE_FreeOperation(g_pOperationHandle);
    g_pOperationHandle = TEE_HANDLE_NULL;
done:
    l_CryptoTaRsa_CleanUpPublicHandle();
    return l_Result;

}




int g_CryptoTaRsa_rsaDecOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen)
{
    TEE_Result l_TeeRetVal = TEE_FAIL;
    int l_Result = OK;

    /** 1) Set the public key object handle */
    l_Result = l_CryptoTaRsa_SetRsaPrivateKey(rsaOper);
    if(FAIL == l_Result)
    {
        TF("Set public key object handle faile!\n");
        goto done;
    }

    /** 7) Allocate the operation handle */
    l_TeeRetVal = TEE_AllocateOperation(&g_pOperationHandle, rsaOper.padding, TEE_MODE_DECRYPT, g_MaxKeySize);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("The allocate operate handle fail, the return value is: 0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto done;
    }
    
    /** 8) Assemble the key object into operation handle */
    l_TeeRetVal = TEE_SetOperationKey(g_pOperationHandle, g_KeyPairObjHandle);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("Set operation key faile, return value is:0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto cleanUp_1;
    } 

    /** 9) Do cipher operation, judge the operation mode to do encrypto or decrypt */
    l_TeeRetVal = TEE_AsymmetricDecrypt(g_pOperationHandle, NULL, 0, inBuf, inLen, outBuf, pOutLen);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("Do AsymmetricDecrypt encrypy operation fail, return valus is:ox%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto cleanUp_1;
    }
    else
    {
        TF("The out put length is:%d\n", *pOutLen);
        g_TA_Printf(outBuf, *pOutLen);
        l_Result = OK;
        goto done;
    }

cleanUp_1:
    TEE_FreeOperation(g_pOperationHandle);
    g_pOperationHandle = TEE_HANDLE_NULL;
done:
    l_CryptoTaRsa_CleanUpKeyPairObj();
    return l_Result;
}






int g_CryptoTaRsa_rsaSignOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen)
{
    TEE_Result l_TeeRetVal = TEE_FAIL;
    int l_Result = FAIL;
    CHAR l_Hash[20] = {0};


    /** 1) Calculate hash value of input data */
    g_CryptoTaHash_sha(EN_OP_SHA1, inBuf, inLen, l_Hash, pOutLen);
    TF("The output hash valus is\n");
    g_TA_Printf(l_Hash, *pOutLen);
    *pOutLen = 0U;

    /** 2) Set the public key object handle */
    l_Result = l_CryptoTaRsa_SetRsaPrivateKey(rsaOper);
    if(FAIL == l_Result)
    {
        TF("Set public key object handle faile!\n");
        goto done;
    }

    /** 3) Allocate the operation handle */
    l_TeeRetVal = TEE_AllocateOperation(&g_pOperationHandle, rsaOper.padding, TEE_MODE_SIGN, g_MaxKeySize);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("The allocate operate handle fail, the return value is: 0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto done;
    }

    /** 4) Assemble the key object into operation handle */
    l_TeeRetVal = TEE_SetOperationKey(g_pOperationHandle, g_KeyPairObjHandle);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("Set operation key faile, return value is:0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto cleanUp_1;
    } 

    *pOutLen = rsaOper.Nlen;
    /** 5) Do cipher operation, judge the operation mode to do encrypto or decrypt */
    l_TeeRetVal = TEE_AsymmetricSignDigest(g_pOperationHandle, NULL, 0, l_Hash, 20, outBuf, pOutLen);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        l_Result = FAIL;
    }
    TF("The Sign output len:%d\n", (*pOutLen));
    g_TA_Printf(outBuf, *pOutLen);

cleanUp_1:
    TEE_FreeOperation(g_pOperationHandle);
    g_pOperationHandle = TEE_HANDLE_NULL;
done:
    l_CryptoTaRsa_CleanUpKeyPairObj();
    return l_Result;
}









int g_CryptoTaRsa_rsaVerifyOper(RsaKey rsaOper, CHAR* inBuf, 
                              CHAR* outBuf, UINT32 inLen, UINT32* pOutLen)
{
    TEE_Result l_TeeRetVal = TEE_FAIL;
    int l_Result = FAIL;
    CHAR l_Hash[20] = {0};

    TF("The input length is :%d\n", inLen);
    TF("The signature info just like follow:\n");
    g_TA_Printf(inBuf, inLen);
    /** 1) Calculate hash value of input data */
    g_CryptoTaHash_sha(EN_OP_SHA1, g_RsaRaw, sizeof(g_RsaRaw), l_Hash, pOutLen);
    TF("The output hash valus is\n");
    g_TA_Printf(l_Hash, *pOutLen);
    *pOutLen = 0U;

    /** 2) Set the public key object handle */
    l_Result = l_CryptoTaRsa_SetRsaPublicKey(rsaOper);
    if(FAIL == l_Result)
    {
        TF("[verify]Set public key object handle faile!\n");
        goto done;
    }

    /** 3) Allocate the operation handle */
    l_TeeRetVal = TEE_AllocateOperation(&g_pOperationHandle, rsaOper.padding, TEE_MODE_VERIFY, g_MaxKeySize);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("[verify]The allocate operate handle fail, the return value is: 0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto done;
    }

    /** 4) Assemble the key object into operation handle */
    l_TeeRetVal = TEE_SetOperationKey(g_pOperationHandle, g_PublicKeyObjHandle);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("[verify]Set operation key faile, return value is:0x%x\n", l_TeeRetVal);
        l_Result = FAIL;
        goto cleanUp_1;
    } 

    *pOutLen = rsaOper.Nlen;
    /** 5) Do cipher operation, judge the operation mode to do encrypto or decrypt */
    l_TeeRetVal = TEE_AsymmetricVerifyDigest(g_pOperationHandle, NULL, 0, l_Hash, 20, inBuf, inLen);
    TF("Return value of VerifyDigest is: 0x%x\n", l_TeeRetVal);
    if(TEE_SUCCESS != l_TeeRetVal)
    {
        TF("Verify faile\n");
        TF("The Sign output len:%d\n", sizeof(VERIFY_RESULT_FAIL));
        *pOutLen = sizeof(VERIFY_RESULT_FAIL);
        TEE_MemMove(outBuf, VERIFY_RESULT_FAIL, sizeof(VERIFY_RESULT_FAIL));
        l_Result = FAIL;
    }
    else
    {
        TF("Verify successful\n");
        TF("The Sign output len:%d\n", sizeof(VERIFY_RESULT_SUCCESS));
        *pOutLen = sizeof(VERIFY_RESULT_FAIL);
        TEE_MemMove(outBuf, VERIFY_RESULT_SUCCESS, sizeof(VERIFY_RESULT_SUCCESS));
        l_Result = OK;
    }
    

cleanUp_1:
    TEE_FreeOperation(g_pOperationHandle);
    g_pOperationHandle = TEE_HANDLE_NULL;
done:
    l_CryptoTaRsa_CleanUpPublicHandle();
    return l_Result;
}






/**
 * @}
 */
