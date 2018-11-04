/*
 ****************************************************************************************
 *
 *               secStorTaDebug.c
 *
 * Filename      : secStorTaDebug.c
 * Programmer(s) : system BSP
 * Filename      : secStorTaDebug.c
 * Author        : Shuai Fengyun
 * Mail          : shuaifengyun@126.com
 * Create Time   : Mon 19 Jun 2017 10:57:16 AM CST
 ****************************************************************************************
 */

#define MOUDLE_SST_DEBUG_TA_C_

/** @defgroup MODULE_NAME_INFOR
* @{
*/

/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "secStorTaDebug.h"




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
void g_TA_Printf(CHAR* buf, UINT32 len);





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
 *- #Description  This function for printf out the data
 * @param   pMsg           [IN] The received request message
 *                               - Type: MBX_Msg *
 *                               - Range: N/A.
 *
 * @return     void
 * @retval     void
 *
 *
 */
void g_TA_Printf(CHAR* buf, UINT32 len)
{
    UINT32 index = 0U;
    for(index = 0U; index < len; index++)
    {
        if(index < 15U)
        {
        }
        else if(0U == index%16U)
        {
            TF("\n");
        }
        else
        {
        }
        
        TF("0x%02x, ", (buf[index] & 0xFFU));

    }
    TF("\n\n");
}





















/**
 * @}
 */
