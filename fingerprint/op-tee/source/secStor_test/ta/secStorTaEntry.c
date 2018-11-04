
/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               secStorTaEntry.c
 *
 * Filename      : secStorTaEntry.c
 * Author        : Shuai Fengyun
 * Mail          : shuaifengyun@126.cn
 * Create Time   : Mon 19 Jun 2017 10:34:39 AM CST
 ****************************************************************************************
 */

#define MOUDLE_SST_ENTRY_TA_C_

/** @defgroup MODULE_NAME_INFOR
* @{
*/

/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "secStorTaHandle.h"





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
    TF("Sec storage task test TA_CreateEntryPoint \n");
    
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
    TF("Sec storage task test TA_OpenSessionEntryPoint\n");

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
    TF("Sec storage task test TA_CloseSessionEntryPoint\n");
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
    TF("Sec storage task test TA_DestroyEntryPoint\n");
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
    UNUSED(paramTypes);

    TF("CMD_ID = %d\n", cmd_id);

    /** 1) Handle the CA request message according to command index
    *      - EN_CMD_AES_KEY_GET: Get the aes boot key;
    *      - Default:            discard the message */
    switch(cmd_id)
    {
        case CMD_TEST:
            l_ret = store_data_sample();
            break;
        case CMD_CREATE_OPER:
            l_ret = g_SecStorTa_CreateFile(paramTypes, params);
            break;
        case CMD_READ_OPER:
            l_ret = g_SecStorTa_Read(paramTypes, params);
            break;
        case CMD_WRITE_OPER:
            l_ret = g_SecStorTa_Write(paramTypes, params);
            break;
        case CMD_TRUNCATE_OPER:
            l_ret = g_SecStorTa_Truncate(paramTypes, params);
            break;
        case CMD_RENAME_OPER:
            l_ret = g_SecStorTa_Rename(paramTypes, params);
            break;
        case CMD_DEL_OPER:
            l_ret = g_SecStorTa_Dle(paramTypes, params);
            break;   
        default:
            l_ret = TEE_EXEC_FAIL;
            break;
    }

    /**3) Return the result */
    return  l_ret;
}


























/**
 * @}
 */
