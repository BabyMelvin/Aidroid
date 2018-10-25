/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyTaEntry.c
 *
 * Filename      : CryptoVerifyTaEntry.c
 * Author        : Shuai Fengyun
 * Mail          : shuaifengyun@126.com
 * Create Time   : Fri 30 Oct 2015 02:31:02 AM EDT
 ****************************************************************************************
 */

#define MOUDLE_CRYPTO_TA_ENTRY_C_

/** @defgroup MODULE_NAME_INFOR
* @{
*/

/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "CryptoVerifyTaHandle.h"
#include "CryptoVerifyTaRsa.h"



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
 *- #Description  This function creating the entry point of crypto verify task.
 * @param   void
 *
 * @return     TEE_Result
 * @retval     TEE_SUCCESS
 *
 *
 */
TEE_Result TA_CreateEntryPoint(void)
{
    TF("Crypto verify task TA_CreateEntryPoint \n");
    
    return TEE_SUCCESS;
}







/** @ingroup MOUDLE_NAME_C_
 *- #Description  This function openning the session of crypto verify task.
 * @param   void
 *
 * @return     TEE_Result
 * @retval     TEE_SUCCESS
 *
 *
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t paramTypes, TEE_Param __maybe_unused params[4], 
                void __maybe_unused **sessionContext)
{
    TEE_Result ret=TEE_SUCCESS;
    TF("Crypto verify task TA_OpenSessionEntryPoint\n");

    UNUSED(paramTypes);
    UNUSED(params);
    UNUSED(sessionContext);

    return ret;
}



/** @ingroup MOUDLE_NAME_C_
 *- #Description  This function closing the seccsion of crypto verify task.
 * @param   void
 *
 * @return     TEE_Result
 * @retval     TEE_SUCCESS
 *
 *
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *session_context)
{
    TF("Crypto verify task TA_CloseSessionEntryPoint\n");
    UNUSED(session_context);
}



/** @ingroup MOUDLE_NAME_C_
 *- #Description  This function for destroying the task of crypto verify.
 * @param   void
 *
 * @return     TEE_Result
 * @retval     TEE_SUCCESS
 *
 *
 */

void TA_DestroyEntryPoint(void)
{
    TF("Crypto verify task TA_DestroyEntryPoint\n");
}







/** @ingroup MOUDLE_NAME_C_
 *- #Description  This function for handling the command in crypto verify task.
 * @param   void
 *
 * @return     TEE_Result
 * @retval     TEE_SUCCESS
 *
 *
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *session_context,
                uint32_t cmd_id, 
                uint32_t paramTypes, TEE_Param params[4])
{
    TEE_Result l_ret = TEE_SUCCESS;
    int l_RetVal = FAIL;

    TF("CMD_ID = %d\n", cmd_id);

    /** 1) Handle the CA request message according to command index
    *      - EN_CMD_AES_KEY_GET: Get the aes boot key;
    *      - Default:            discard the message */
    switch(cmd_id)
    {
        case CMD_GEN_RANDOM_OPER: 
            TF("Start to generate random data!!!!\n");
            l_RetVal= g_CryptoTaHandle_Random(paramTypes, params);
            break;
        case CMD_SHA_OPER:
            TF("Entry the sha oper\n");
            l_RetVal = g_CryptoTaHandle_Sha(paramTypes, params);
            break;
        case CMD_AES_OPER:
            TF("Entry the aes operation!!!\n");
            l_RetVal = g_CryptoTaHandle_Aes(paramTypes, params);
            break;
        case CMD_PBKDF_OPER:
            TF("Entry pbkdf operation!!\n");
            l_RetVal = g_CryptoTaHandle_Pbkdf(paramTypes, params);
            break;
        case CMD_RSA_ENC_PKCS1_OPER:
            TF("Entry RSA1024_ENC_PKCS1!\n");
            l_RetVal = g_CryptoTaHandle_Rsa(paramTypes, params, EM_OP_ENC, PADDING_PKCS1);
            break;
        case CMD_RSA_DEC_PKCS1_OPER:
            TF("Entry RSA1024_DEC_PKCS1!\n");
            l_RetVal = g_CryptoTaHandle_Rsa(paramTypes, params, EM_OP_DEC, PADDING_PKCS1);
            break;
        case CMD_RSA_SIGN_PKCS1_OPER:
            TF("Entry RSA1024_SIGN_PKCS1!\n");
            l_RetVal = g_CryptoTaHandle_Rsa(paramTypes, params, EM_OP_SIGN, PADDING_PKCS1);
            break;
        case CMD_RSA_VERIFY_PKCS1_OPER:
            TF("Entry RSA1024_VERIFY_PKCS1!\n");
            l_RetVal = g_CryptoTaHandle_Rsa(paramTypes, params, EM_OP_VERIFY, PADDING_PKCS1);
            break;
        case CMD_HMAC_OPER:
            TF("Entry HMAC!\n");
            l_RetVal = g_CryptoTaHandle_hmac(paramTypes, params);
            break;
        case CMD_BASE64_OPER:
            TF("Entry BASE64!\n");
            l_RetVal = g_CryptoTaHandle_base64(paramTypes, params);
            break;

        default:
            l_RetVal = FAIL;
            break;
    }

    /**2) Check if the crypto operation is successful */
    if(FAIL == l_RetVal)
    {
        l_ret = TEE_FAIL;
    }
    else
    {
        l_ret = TEE_SUCCESS;
    }

    /**3) Return the result */
    return  l_ret;
}








/**
 * @}
 */
