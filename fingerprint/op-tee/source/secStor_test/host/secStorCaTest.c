
/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               secStorCaTest.c
 *
 * Filename      : secStorCaTest.c
 * Author        : Shuai Fengyun
 * Mail          : shuaifengyun@126.cn
 * Create Time   : Mon 19 Jun 2017 10:33:32 AM CST
 ****************************************************************************************
 */

#define MOUDLE_SST_TEST_CA_C_

/** @defgroup MODULE_NAME_INFOR
* @{
*/

/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "secStorCaType.h"
#include "secStorCaDebug.h"
#include "secStorCaTest.h"
#include "secStorCaHandle.h"
#include "stdio.h"


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

CHAR oldFileName[] = "secureFile.txt";
CHAR newFileName[] = "changeSecureFile.txt";

CHAR readBuf[256] = {0};
CHAR writeBuf[] = "This is the test data which need be wrote into secure file";



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
int main(int argc, char *argv[])
{
    UINT32 fd = 0xFFFF;
    int l_Ret = FAIL;
    UINT32 l_FileLen = 0U;



    TF("create operation!\n");
    /** 2) Read data from secure file */
    l_Ret = g_SecStorCa_CreateFile(sizeof(oldFileName), oldFileName);
    if(FAIL == l_Ret)
    {
        TF("Read secure file fail\n");
        return 0;
    }

    TF("write operation!\n");
    /** 3) Write data into secure file */
    l_Ret = g_SecStorCa_WiteFile(sizeof(oldFileName), oldFileName, sizeof(writeBuf), writeBuf);
    if(FAIL == l_Ret)
    {
        TF("Write secure file fail\n");
        return 0;
    }

    TF("read operation!\n");
    /** 2) Read data from secure file */
    l_Ret = g_SecStorCa_ReadFile(sizeof(oldFileName), oldFileName, 60U, readBuf);
    if(FAIL == l_Ret)
    {
        TF("Read secure file fail\n");
        return 0;
    }
    else
    {
        g_CA_PrintfBuffer(readBuf, 60U);
        TF("The read data is:\n");
        TF("%s\n", readBuf);
    }


    TF("rename operation!\n");
    l_Ret = g_SecStorCa_RenameFile(sizeof(oldFileName), oldFileName, sizeof(newFileName), newFileName);
    if(FAIL == l_Ret)
    {
        TF("Read secure file fail\n");
        return 0;
    }


    TF("read operation!\n");
    memset(readBuf, 0, 256);
    /** 2) Read data from secure file */
    l_Ret = g_SecStorCa_ReadFile(sizeof(newFileName), newFileName, 60U, readBuf);
    if(FAIL == l_Ret)
    {
        TF("Read secure file fail\n");
        return 0;
    }
    else
    {
        g_CA_PrintfBuffer(readBuf, 60U);
        TF("The read data is:\n");
        TF("%s\n", readBuf);
    }



    TF("truncate operation!\n");
    /** 6) Truncate the secure file */
    l_Ret = g_SecStorCa_TrunCatFile(sizeof(newFileName), newFileName, 20);
    if(FAIL == l_Ret)
    {
        TF("Write secure file fail\n");
        return 0;
    }


    TF("read3 operation!\n");
    memset(readBuf, 0, 256);
    l_Ret = g_SecStorCa_ReadFile(sizeof(newFileName), newFileName, 60U, readBuf);
    if(FAIL == l_Ret)
    {
        TF("Read secure file fail\n");
        return 0;
    }
    else
    {
        g_CA_PrintfBuffer(readBuf, 60U);
        TF("The read data is:\n");
        TF("%s\n", readBuf);
    }



    TF("Delete operation!\n");
    /** 10) Delete secure file */
    l_Ret = g_SecStorCa_DeleteFile(sizeof(newFileName), newFileName);
    if(FAIL == l_Ret)
    {
        TF("Write secure file fail\n");
        return 0;
    }
}






















/**
 * @}
 */
