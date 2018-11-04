
/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyTaAes.c
 *
 * Filename      : CryptoVerifyTaAes.c
 * Author        : Shuai Fengyun
 * Mail          : shuaifengyun@126.com
 * Create Time   : Fri 30 Oct 2015 12:40:02 AM EDT
 ****************************************************************************************
 */

#define MOUDLE_CRYPTO_TA_AES_C_

/** @defgroup MODULE_NAME_INFOR
* @{
*/

/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "CryptoVerifyTaAes.h"
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
int g_CryptoTaAes_AesOper(AesOperation aesOper);





/*
 *******************************************************************************
 *                          FUNCTIONS USED ONLY BY THIS MODULE
 *******************************************************************************
*/





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






int g_CryptoTaAes_AesOper(AesOperation aesOper)
{
    TEE_OperationHandle l_pOperation = NULL;
    TEE_ObjectHandle l_pKeyObj = NULL;
    TEE_Attribute l_pAttr;
    CHAR* l_pInbuf = aesOper.inBuf;
    CHAR* l_pOutbuf = aesOper.outBuf;
    UINT32 l_dataLen = aesOper.dataLen;
    TEE_Result l_RetVal = TEE_FAIL;
    int l_Result = FAIL;


    TF("The Aes operation information just like follow:\n");
    TF("Aes key=\n");
    g_TA_Printf(aesOper.key, aesOper.keyLen);
    TF("IV=\n");
    g_TA_Printf(aesOper.iv, aesOper.ivLen);
    TF("Algorith= 0x%x\n", aesOper.algorithmId);
    TF("Mode=0x%x\n", aesOper.operMode);
    TF("Raw just like follow:\n");
    g_TA_Printf(aesOper.inBuf, aesOper.dataLen);

    /**1) Allocate the operation handle */
    l_RetVal = TEE_AllocateOperation(&l_pOperation, aesOper.algorithmId, aesOper.operMode, aesOper.keyLen);
    if(TEE_SUCCESS != l_RetVal)
    {
        l_Result = FAIL;
        goto cleanup_1;
    }

    TF("Allocate object\n");
    /**2) Allocate the object handle */
    l_RetVal = TEE_AllocateTransientObject(TEE_TYPE_AES, aesOper.keyLen, &l_pKeyObj);
    if(TEE_SUCCESS != l_RetVal)
    {
        l_Result = FAIL;
        goto cleanup_1;
    }   

    TF("Init attribute\n");
    /**3) Set the key object parameter */
    TEE_InitRefAttribute(&l_pAttr, TEE_ATTR_SECRET_VALUE, aesOper.key, 16);
    l_RetVal = TEE_PopulateTransientObject(l_pKeyObj, &l_pAttr, 1);
    if(TEE_SUCCESS != l_RetVal)
    {
        l_Result = FAIL;
        goto cleanup_1;
    }

    TF("Set key\n");
    /**4) Assemble aes operation handle */
    l_RetVal = TEE_SetOperationKey(l_pOperation, l_pKeyObj);
    if(TEE_SUCCESS != l_RetVal)
    {
        l_Result = FAIL;
        goto cleanup_2;
    }

    TF("Init cipher\n");
    /**5) Initialze cipher operation */
    TEE_CipherInit(l_pOperation, aesOper.iv, aesOper.ivLen);


#if 0
    /**6) Update the input data into the buffer for do AES operation */
    while(true)
    {
        if(SIZE_OF_AES128_BLOCK_LEN == l_dataLen)
        {
            break;
        }
        else
        {
            if(0U != (l_dataLen / SIZE_OF_AES128_BLOCK_LEN))
            {
                /* Do update operation */
                l_RetVal = TEE_CipherUpdate(l_pOperation, l_pInbuf, SIZE_OF_AES128_BLOCK_LEN,
                                              l_pOutbuf, SIZE_OF_AES128_BLOCK_LEN);
                if(TEE_SUCCESS != l_RetVal)
                {
                    l_Result = FAIL;
                    goto cleanup_2;
                }

                /* Move the buffer point & length of remainder data */
                l_pInbuf = &(l_pInbuf[SIZE_OF_AES128_BLOCK_LEN]);
                l_pOutbuf = &(l_pOutbuf[SIZE_OF_AES128_BLOCK_LEN]);
                l_dataLen = l_dataLen - SIZE_OF_AES128_BLOCK_LEN;
            }
            else
            {
                break;
            }
            
        }
    }
#endif

    TF("Do final cipher\n");
    /** 6) Do the final AES operation */
    l_RetVal = TEE_CipherDoFinal(l_pOperation, l_pInbuf, l_dataLen, l_pOutbuf, &l_dataLen);
    if(TEE_SUCCESS != l_RetVal)
    {
        l_Result = FAIL;
    }
    else
    {
        l_Result = OK;
    }

    TF("The aes operation out put just like follow:\n");
    g_TA_Printf(aesOper.outBuf, aesOper.dataLen);



cleanup_2:
    TEE_FreeOperation(l_pOperation);
cleanup_1:
    return l_Result;
}








/**
 * @}
 */
