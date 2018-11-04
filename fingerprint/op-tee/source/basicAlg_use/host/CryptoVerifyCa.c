
/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyCa.c
 *
 * Filename      : CryptoVerifyCa.c
 * Author        : Shuai Fengyun
 * Mail          : shuai.fengyun@126.com
 * Create Time   : Wed 11 Nov 2015 03:10:11 PM CST
 ****************************************************************************************
 */

#define MOUDLE_CRYPTO_VERIFY_CA_C_

/** @defgroup MODULE_NAME_INFOR
* @{
*/

/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "CryptoVerifyCa.h"
#include "CryptoVerifyCaDebug.h"




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
static int g_TaskInitFlag = -1;    /* Flag if the task done initialize operation */
TEEC_UUID svc_id = CRYPTO_VERIFY_UUID_ID;
TEEC_Context g_TaskContext;




/*
 *******************************************************************************
 *                               FUNCTIONS IMPLEMENT
 *******************************************************************************
*/

static int l_CryptoVerifyCa_TaskInit(void)
{
    TEEC_Result result;
    int l_RetVal = OK;
    
    /**1) Check if need to do task initialization operation */
    if(-1 == g_TaskInitFlag)
    {
        result = TEEC_InitializeContext(NULL, &g_TaskContext);
        if(result != TEEC_SUCCESS) 
        {
            TF("InitializeContext failed, ReturnCode=0x%x\n", result);
            l_RetVal= FAIL;
        } 
        else 
        {
            g_TaskInitFlag = 1;
            TF("InitializeContext success\n");
            l_RetVal = OK;
        }
    }
    
    return l_RetVal;
}


static int l_CryptoVerifyCa_OpenSession(TEEC_Session* session)
{
    TEEC_Result result;
    int l_RetVal = FAIL;
    uint32_t origin;

    result = TEEC_OpenSession(&g_TaskContext, session, &svc_id, 
                                TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if(result != TEEC_SUCCESS) 
    {
        TF("OpenSession failed, ReturnCode=0x%x, ReturnOrigin=0x%x\n", result, origin);
        g_TaskInitFlag = -1;
        l_RetVal = FAIL;
    } 
    else 
    {
        TF("OpenSession success\n");
        l_RetVal = OK;
    }

    return l_RetVal;
}


static int l_CryptoVerifyCa_SendCommand(TEEC_Operation* operation, TEEC_Session* session, uint32_t commandID)
{
    TEEC_Result result;
    int l_RetVal = FAIL;
    uint32_t origin;

    result = TEEC_InvokeCommand(session, commandID, operation, &origin);
    if (result != TEEC_SUCCESS) 
    {
        TF("InvokeCommand failed, ReturnCode=0x%x, ReturnOrigin=0x%x\n", result, origin);
        l_RetVal = FAIL;
    } 
    else 
    {
        TF("InvokeCommand success\n");
        l_RetVal = OK;
    }


    return l_RetVal;
}






int g_CryptoVerifyCa_Random(UINT32 len, CHAR* output)
{
    TEEC_Session   l_session;    /* Define the session of TA&CA */
    TEEC_Operation l_operation;  /* Define the operation for communicating between TA&CA */
    int l_RetVal = FAIL;       /* Define the return value of function */

    /**1) Initialize this task */
    l_RetVal = l_CryptoVerifyCa_TaskInit();
    if(FAIL == l_RetVal)
    {
        goto cleanup_1;
    }

    /**2) Open session */
    l_RetVal = l_CryptoVerifyCa_OpenSession(&l_session);
    if(FAIL == l_RetVal)
    {
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&l_operation, 0x0, sizeof(TEEC_Operation));
    l_operation.started = 1;
    l_operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT,TEEC_NONE, 
                                              TEEC_NONE, TEEC_NONE);
    l_operation.params[0].tmpref.size = len;
    l_operation.params[0].tmpref.buffer = output;

    /**4) Send command to TA */
    l_RetVal = l_CryptoVerifyCa_SendCommand(&l_operation, &l_session, CMD_GEN_RANDOM_OPER);
    if(FAIL == l_RetVal)
    {
        goto cleanup_3;
    }

    /**5) The clean up operation */
    cleanup_3:
        TEEC_CloseSession(&l_session);
    cleanup_2:
        TEEC_FinalizeContext(&g_TaskContext);
    cleanup_1:
        return l_RetVal;
}



int g_CryptoVerifyCa_Sha(CHAR* pData, UINT32 len, EN_SHA_MODE shaMode, CHAR* output, UINT32 outLen)
{
    TEEC_Session   l_session;    /* Define the session of TA&CA */
    TEEC_Operation l_operation;  /* Define the operation for communicating between TA&CA */
    int l_RetVal = FAIL;       /* Define the return value of function */

    /**1) Initialize this task */
    l_RetVal = l_CryptoVerifyCa_TaskInit();
    if(FAIL == l_RetVal)
    {
        goto cleanup_1;
    }

    /**2) Open session */
    l_RetVal = l_CryptoVerifyCa_OpenSession(&l_session);
    if(FAIL == l_RetVal)
    {
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&l_operation, 0x0, sizeof(TEEC_Operation));
    l_operation.started = 1;
    l_operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,TEEC_VALUE_INPUT, 
                                              TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
    l_operation.params[0].tmpref.size = len;
    l_operation.params[0].tmpref.buffer = pData;
    l_operation.params[1].value.a = shaMode;
    l_operation.params[2].tmpref.size = outLen;
    l_operation.params[2].tmpref.buffer = output;

    /**4) Send command to TA */
    l_RetVal = l_CryptoVerifyCa_SendCommand(&l_operation, &l_session, CMD_SHA_OPER);
    TF("The respond data length is 0x%02x\n", outLen);
    if(FAIL == l_RetVal)
    {
        goto cleanup_3;
    }

    /**5) The clean up operation */
    cleanup_3:
        TEEC_CloseSession(&l_session);
    cleanup_2:
        TEEC_FinalizeContext(&g_TaskContext);
    cleanup_1:
        return l_RetVal;
}


int g_CryptoVerifyCa_hmac(UINT32 len, CHAR* output, UINT32 count)
{
    TEEC_Session   l_session;    /* Define the session of TA&CA */
    TEEC_Operation l_operation;  /* Define the operation for communicating between TA&CA */
    int l_RetVal = FAIL;       /* Define the return value of function */

    /**1) Initialize this task */
    l_RetVal = l_CryptoVerifyCa_TaskInit();
    if(FAIL == l_RetVal)
    {
        goto cleanup_1;
    }

    /**2) Open session */
    l_RetVal = l_CryptoVerifyCa_OpenSession(&l_session);
    if(FAIL == l_RetVal)
    {
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&l_operation, 0x0, sizeof(TEEC_Operation));
    l_operation.started = 1;
    l_operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INOUT,TEEC_VALUE_INPUT, 
                                              TEEC_NONE, TEEC_NONE);
    l_operation.params[0].tmpref.size = len;
    l_operation.params[0].tmpref.buffer = output;
    l_operation.params[1].value.a = count;

    /**4) Send command to TA */
    l_RetVal = l_CryptoVerifyCa_SendCommand(&l_operation, &l_session, CMD_HMAC_OPER);
    TF("The respond data length is 0x%02x\n", len);
    if(FAIL == l_RetVal)
    {
        goto cleanup_3;
    }

    /**5) The clean up operation */
    cleanup_3:
        TEEC_CloseSession(&l_session);
    cleanup_2:
        TEEC_FinalizeContext(&g_TaskContext);
    cleanup_1:
        return l_RetVal;
}



int g_CryptoVerifyCa_Aes(CHAR* pData, UINT32 len, EN_AES_MODE aesMode, 
                         EN_AES_OPERATION_ACTION operAction, CHAR* output)
{
    TEEC_Session   l_session;    /* Define the session of TA&CA */
    TEEC_Operation l_operation;  /* Define the operation for communicating between TA&CA */
    AesOperModeInfo l_aesMode;
    int l_RetVal = FAIL;       /* Define the return value of function */

    /**1) Initialize this task */
    l_RetVal = l_CryptoVerifyCa_TaskInit();
    if(FAIL == l_RetVal)
    {
        goto cleanup_1;
    }

    /**2) Open session */
    l_RetVal = l_CryptoVerifyCa_OpenSession(&l_session);
    if(FAIL == l_RetVal)
    {
        goto cleanup_2;
    }

    l_aesMode.active = operAction;
    l_aesMode.mode = aesMode;

    /**3) Set the communication context between CA&TA */
    memset(&l_operation, 0x0, sizeof(TEEC_Operation));
    l_operation.started = 1;
    l_operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT, 
                                              TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INPUT);
    l_operation.params[0].value.a = operAction;
    l_operation.params[0].value.b = aesMode;
    l_operation.params[1].tmpref.size = len;
    l_operation.params[1].tmpref.buffer = pData;
    l_operation.params[2].tmpref.size = len;
    l_operation.params[2].tmpref.buffer = output;
    l_operation.params[3].value.a = len;

    /**4) Send command to TA */
    l_RetVal = l_CryptoVerifyCa_SendCommand(&l_operation, &l_session, CMD_AES_OPER);
    TF("The respond data length is 0x%02x\n", len);
    
    if(FAIL == l_RetVal)
    {
        goto cleanup_3;
    }

    /**5) The clean up operation */
    cleanup_3:
        TEEC_CloseSession(&l_session);
    cleanup_2:
        TEEC_FinalizeContext(&g_TaskContext);
    cleanup_1:
        return l_RetVal;
}




int g_CryptoVerifyCa_Pbkdf(UINT32 len, CHAR* output)
{
    TEEC_Session   l_session;    /* Define the session of TA&CA */
    TEEC_Operation l_operation;  /* Define the operation for communicating between TA&CA */
    int l_RetVal = FAIL;       /* Define the return value of function */

    /**1) Initialize this task */
    l_RetVal = l_CryptoVerifyCa_TaskInit();
    if(FAIL == l_RetVal)
    {
        goto cleanup_1;
    }

    /**2) Open session */
    l_RetVal = l_CryptoVerifyCa_OpenSession(&l_session);
    if(FAIL == l_RetVal)
    {
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&l_operation, 0x0, sizeof(TEEC_Operation));
    l_operation.started = 1;
    l_operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INOUT,TEEC_NONE, 
                                              TEEC_NONE, TEEC_NONE);
    l_operation.params[0].tmpref.size = len;
    l_operation.params[0].tmpref.buffer = output;

    /**4) Send command to TA */
    l_RetVal = l_CryptoVerifyCa_SendCommand(&l_operation, &l_session, CMD_PBKDF_OPER);
    TF("The respond data length is 0x%02x\n", len);
    if(FAIL == l_RetVal)
    {
        goto cleanup_3;
    }

    /**5) The clean up operation */
    cleanup_3:
        TEEC_CloseSession(&l_session);
    cleanup_2:
        TEEC_FinalizeContext(&g_TaskContext);
    cleanup_1:
        return l_RetVal;
}




int g_CryptoVerifyCa_Rsa(RsaCaPara rsaPara)
{
    TEEC_Session   l_session;    /* Define the session of TA&CA */
    TEEC_Operation l_operation;  /* Define the operation for communicating between TA&CA */
    AesOperModeInfo l_aesMode;
    int l_RetVal = FAIL;       /* Define the return value of function */
    TF("The send command id is: %d\n", rsaPara.cmdId);

    /**1) Initialize this task */
    l_RetVal = l_CryptoVerifyCa_TaskInit();
    if(FAIL == l_RetVal)
    {
        goto cleanup_1;
    }

    /**2) Open session */
    l_RetVal = l_CryptoVerifyCa_OpenSession(&l_session);
    if(FAIL == l_RetVal)
    {
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&l_operation, 0x0, sizeof(TEEC_Operation));
    l_operation.started = 1;
    l_operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, 
                                              TEEC_VALUE_INPUT, TEEC_NONE);
    l_operation.params[0].tmpref.size = rsaPara.m_InputLen;
    l_operation.params[0].tmpref.buffer = rsaPara.m_pInput;
    l_operation.params[1].tmpref.size = rsaPara.m_OutputLen;
    l_operation.params[1].tmpref.buffer = rsaPara.m_pOutput;
    l_operation.params[2].value.a = rsaPara.Rsa_Elect;
    

    /**4) Send command to TA */    
    l_RetVal = l_CryptoVerifyCa_SendCommand(&l_operation, &l_session, rsaPara.cmdId);
    TF("The respond data length is 0x%02x\n", rsaPara.m_OutputLen);
    g_CA_PrintfBuffer(rsaPara.m_pOutput, rsaPara.m_OutputLen);
    if(FAIL == l_RetVal)
    {
        goto cleanup_3;
    }

    /**5) The clean up operation */
    cleanup_3:
        TEEC_CloseSession(&l_session);

    cleanup_2:
        TEEC_FinalizeContext(&g_TaskContext);
    cleanup_1:
        return l_RetVal;
}





int g_CryptoVerifyCa_base64(CHAR* input, UINT32 lenIn, CHAR* output, UINT32 lenOut, UINT32 oper)
{
    TEEC_Session   l_session;    /* Define the session of TA&CA */
    TEEC_Operation l_operation;  /* Define the operation for communicating between TA&CA */
    int l_RetVal = FAIL;       /* Define the return value of function */

    /**1) Initialize this task */
    l_RetVal = l_CryptoVerifyCa_TaskInit();
    if(FAIL == l_RetVal)
    {
        goto cleanup_1;
    }

    /**2) Open session */
    l_RetVal = l_CryptoVerifyCa_OpenSession(&l_session);
    if(FAIL == l_RetVal)
    {
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&l_operation, 0x0, sizeof(TEEC_Operation));
    l_operation.started = 1;
    l_operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,TEEC_MEMREF_TEMP_OUTPUT, 
                                              TEEC_VALUE_INPUT, TEEC_NONE);
    l_operation.params[0].tmpref.size = lenIn;
    l_operation.params[0].tmpref.buffer = input;
    l_operation.params[1].tmpref.size = lenOut;
    l_operation.params[1].tmpref.buffer = output;
    l_operation.params[2].value.a = oper;

    /**4) Send command to TA */
    l_RetVal = l_CryptoVerifyCa_SendCommand(&l_operation, &l_session, CMD_BASE64_OPER);
    TF("The respond data length is 0x%02x\n", lenOut);
    if(FAIL == l_RetVal)
    {
        goto cleanup_3;
    }

    /**5) The clean up operation */
    cleanup_3:
        TEEC_CloseSession(&l_session);
    cleanup_2:
        TEEC_FinalizeContext(&g_TaskContext);
    cleanup_1:
        return l_RetVal;
}




/**
 * @}
 */
