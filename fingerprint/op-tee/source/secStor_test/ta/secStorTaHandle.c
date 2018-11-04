
/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               secStorTaHandle.c
 *
 * Filename      : secStorTaHandle.c
 * Author        : Shuai Fengyun
 * Mail          : shuaifengyun@126.cn
 * Create Time   : Mon 19 Jun 2017 10:34:50 AM CST
 ****************************************************************************************
 */

#define MOUDLE_SST_HANDLE_TA_C_

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
TEE_Result g_SecStorTa_CreateFile(uint32_t paramTypes, TEE_Param params[4]);
TEE_Result g_SecStorTa_Read(uint32_t paramTypes, TEE_Param params[4]);
TEE_Result g_SecStorTa_Write(uint32_t paramTypes, TEE_Param params[4]);
TEE_Result g_SecStorTa_Truncate(uint32_t paramTypes, TEE_Param params[4]);
TEE_Result g_SecStorTa_Rename(uint32_t paramTypes, TEE_Param params[4]);
TEE_Result g_SecStorTa_Dle(uint32_t paramTypes, TEE_Param params[4]);
TEE_Result store_data_sample(void);






/*
 *******************************************************************************
 *                          VARIABLES USED ONLY BY THIS MODULE
 *******************************************************************************
*/
 
TEE_ObjectHandle g_FilesObj;





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






TEE_Result store_data_sample(void)
{
    const uint32_t r_flags=TEE_DATA_FLAG_ACCESS_READ;
    const uint32_t w_flags=TEE_DATA_FLAG_ACCESS_WRITE;
    const void* create_objectID="store_data_sample.txt";
    const void* initialData="This a TrustedCore store_data_sample initial data\n";
    TEE_ObjectHandle persistent_data;
    TEE_Result ret;
    char  read_buffer[255];
    size_t read_test_size=49;
    uint32_t count[1];
    uint32_t storageID=TEE_OBJECT_STORAGE_PRIVATE;
    TF("store_data_sample start:============\n");

    /*创建永久Object: flag为写操作(会将initialData和object info和attributes写入物理介质*/
    ret=TEE_CreatePersistentObject(storageID, create_objectID, strlen(create_objectID),w_flags, TEE_HANDLE_NULL , initialData, (size_t)(strlen((char*)initialData)), (&persistent_data));
    if (TEE_SUCCESS != ret)
    {
        TF("Failed to execute TEE_CreatePersistentObject:ret = %x\n", ret);
        goto error;
    }
    TF("Succeed to TEE_CreatePersistentObject for datasteam store.\n");
    TEE_CloseObject(persistent_data);


    /*打开已经创建的文件，进行initial data的读取*/
    ret = TEE_OpenPersistentObject(storageID, create_objectID,strlen(create_objectID), r_flags|TEE_DATA_FLAG_ACCESS_WRITE_META, (&persistent_data));
    if (TEE_SUCCESS != ret)
    {
        TF("Failed to execute TEE_OpenPersistentObject:ret = %x\n", ret);
        goto error;
    }
    TF("Succeed to TEE_OpenPersistentObject for persistent_data.\n");

    /*读取已存入的数据*/
    ret = TEE_ReadObjectData(persistent_data,read_buffer,read_test_size, count);
    if(ret == TEE_SUCCESS)
    {
        read_buffer[*count]='\0';
        /*对读出的数据进行判断，保证读出的数据与创建 object时写入的相同*/
        if (TEE_MemCompare(read_buffer, initialData, read_test_size))
        {
            TF("read content is %s \n",read_buffer);
            TF("flash content is %s \n",(char*)initialData);
            TF("Failed to read buffer.\n");
            goto error;
        }
        if (*count != read_test_size)
        {
            TF("read length(read_test_size=%d) is not equal to count(count=%d).\n", read_test_size, *count);
            goto error;
        }
    }
    else
    {
        TF("Failed to read data for persistent_data.\n");
        goto error;
    }
    TF("Succeed to TEE_ReadObjectData for persistent_data.\n");

    /*删除已经创建的永久 object,保证不影响后续的测试用例*/
    //TEE_CloseAndDeletePersistentObject(persistent_data);
    TEE_CloseObject(persistent_data);

    TF("Succeed to store and delete data in flash:^o^   ^o^\n");
    TF("store_data_sample end:============\n");
    return TEE_SUCCESS;

error:
    TF("Failed to store data in flash.\n");
    TF("store_data_sample end:============\n");
    return TEE_EXEC_FAIL;
}












static TEE_Result l_SecStorTa_Open(CHAR* fileName, UINT32 fileNameSize)
{
    TEE_Result l_ret = TEE_EXEC_FAIL; 
    UINT32 l_AccFlg = TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ;


    l_ret = TEE_OpenPersistentObject(TEE_OBJECT_STORAGE_PRIVATE, fileName, 
                                   fileNameSize, l_AccFlg, (&g_FilesObj));
    if (TEE_SUCCESS != l_ret)
    {        
        return TEE_EXEC_FAIL;
    }
    else
    {
        return TEE_SUCCESS;
    }
}



static TEE_Result l_SecStorTa_OpenForSpe(CHAR* fileName, UINT32 fileNameSize)
{
    TEE_Result l_ret = TEE_EXEC_FAIL; 
    UINT32 l_AccFlg = TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_WRITE_META;


    l_ret = TEE_OpenPersistentObject(TEE_OBJECT_STORAGE_PRIVATE, fileName, 
                                   fileNameSize, l_AccFlg, (&g_FilesObj));
    if (TEE_SUCCESS != l_ret)
    {        
        return TEE_EXEC_FAIL;
    }
    else
    {
        return TEE_SUCCESS;
    }
}


TEE_Result g_SecStorTa_CreateFile(uint32_t paramTypes, TEE_Param params[4])
{
    TEE_Result l_ret = TEE_EXEC_FAIL;
    UINT32 l_fileNameSize = 0U;
    CHAR* l_fileName = NULL;
    UNUSED(paramTypes);

    l_fileName = params[0].memref.buffer;
    l_fileNameSize = params[0].memref.size;

    TF("[CREATE] start to create file: %s\n", l_fileName);
    l_ret = TEE_CreatePersistentObject(TEE_OBJECT_STORAGE_PRIVATE, l_fileName,
                       l_fileNameSize, TEE_DATA_FLAG_ACCESS_WRITE_META | TEE_DATA_FLAG_ACCESS_WRITE, 
                       TEE_HANDLE_NULL , NULL, 0, 
                       (&g_FilesObj));
    if (TEE_SUCCESS != l_ret)
    {
        TF("[CREATE] create file fail");
        return TEE_EXEC_FAIL;
    }
    else
    {
        TEE_CloseObject(g_FilesObj);
        return TEE_SUCCESS;
    }
}


TEE_Result g_SecStorTa_Read(uint32_t paramTypes, TEE_Param params[4])
{
    TEE_Result l_ret = TEE_SUCCESS;
    CHAR* l_OutBuf = NULL;
    UINT32 l_ReadLen = 0U;
    CHAR* l_FileName = NULL;
    UINT32 l_FileNameSize = 0U;
    UINT32 l_Count = 0U;
    UNUSED(paramTypes);

    
    /** 1) Get the fd of secure file */
    l_OutBuf = params[0].memref.buffer;
    l_ReadLen = params[0].memref.size;
    l_FileName = params[1].memref.buffer;
    l_FileNameSize = params[1].memref.size;

    TF("[READ] start to read file: %s\n", l_FileName);
    l_ret = l_SecStorTa_Open(l_FileName, l_FileNameSize);
    if (TEE_SUCCESS != l_ret)
    {        
        TF("[READ] open file fail\n");
        return TEE_EXEC_FAIL;
    }

    /** 2) Start read data from secure file */
    l_ret = TEE_ReadObjectData(g_FilesObj, l_OutBuf, l_ReadLen, &l_Count);

    TEE_CloseObject(g_FilesObj);
    if (TEE_SUCCESS != l_ret)
    {        
        TF("[READ] read file fail\n");
        return TEE_EXEC_FAIL;
    }
    else
    {
        return TEE_SUCCESS;
    }
}



TEE_Result g_SecStorTa_Write(uint32_t paramTypes, TEE_Param params[4])
{
    TEE_Result l_ret = TEE_SUCCESS;
    CHAR* l_InBuf = NULL;
    UINT32 l_WriteLen = 0U;
    CHAR* l_FileName = NULL;
    UINT32 l_FileNameSize = 0U;
    UNUSED(paramTypes);

    
    /** 1) Get the fd of secure file */
    l_InBuf = params[0].memref.buffer;
    l_WriteLen = params[0].memref.size;
    l_FileName = params[1].memref.buffer;
    l_FileNameSize = params[1].memref.size;

    TF("[WRITE] start to write file: %s\n", l_FileName);
    l_ret = l_SecStorTa_Open(l_FileName, l_FileNameSize);
    if (TEE_SUCCESS != l_ret)
    {     
        TF("[WRITE] open file fail\n");
        return TEE_EXEC_FAIL;
    }

    /** 2) Start read data from secure file */
    l_ret = TEE_WriteObjectData(g_FilesObj, l_InBuf, l_WriteLen);

    TEE_CloseObject(g_FilesObj);
    if (TEE_SUCCESS != l_ret)
    {        
        TF("[WRITE] wtire file fail\n");
        return TEE_EXEC_FAIL;
    }
    else
    {
        return TEE_SUCCESS;
    }
}






TEE_Result g_SecStorTa_Truncate(uint32_t paramTypes, TEE_Param params[4])
{
    TEE_Result l_ret = TEE_SUCCESS;
    CHAR* l_FileName = NULL;
    UINT32 l_FileNameLen = 0U;
    UINT32 l_FinSize = 0U;
    UNUSED(paramTypes);

    
    /** 1) Get the fd of secure file */
    l_FinSize = params[0].value.a;
    l_FileName = params[1].memref.buffer;
    l_FileNameLen = params[1].memref.size;

    TF("[TRUN] start to truncate file: %s\n", l_FileName);
    l_ret = l_SecStorTa_OpenForSpe(l_FileName, l_FileNameLen);
    if (TEE_SUCCESS != l_ret)
    {        
        TF("[TRUN] open file fail\n");
        return TEE_EXEC_FAIL;
    }

    TF("[TRUN] Start to truncate file:%s,%d\n", l_FileName, l_FinSize);
    /** 2) Start read data from secure file */
    l_ret = TEE_TruncateObjectData(g_FilesObj, l_FinSize);
    
    TEE_CloseObject(g_FilesObj);
    if (TEE_SUCCESS != l_ret)
    {        
        TF("[TRUN] turncate file fail\n");
        return TEE_EXEC_FAIL;
    }
    else
    {
        
        return TEE_SUCCESS;
    }
}



TEE_Result g_SecStorTa_Rename(uint32_t paramTypes, TEE_Param params[4])
{
    TEE_Result l_ret = TEE_SUCCESS;
    CHAR* l_oldName = NULL;
    UINT32 l_oldLen = 0U;
    CHAR* l_newName = NULL;
    UINT32 l_newLen = 0U;
    UNUSED(paramTypes);

    
    /** 1) Get the fd of secure file */
    l_oldName = params[0].memref.buffer;
    l_oldLen = params[0].memref.size;
    l_newName = params[1].memref.buffer;
    l_newLen = params[1].memref.size;

    TF("[RENAME] start to rename file: %s\n", l_oldName);
    l_ret = l_SecStorTa_OpenForSpe(l_oldName, l_oldLen);
    if (TEE_SUCCESS != l_ret)
    {        
        TF("[RENAME] open file fail\n");
        return TEE_EXEC_FAIL;
    }


    /** 2) Start read data from secure file */
    l_ret = TEE_RenamePersistentObject(g_FilesObj, l_newName, l_newLen);

    TEE_CloseObject(g_FilesObj);
    if (TEE_SUCCESS != l_ret)
    {        
        TF("[RENAME] rename file fail\n");
        return TEE_EXEC_FAIL;
    }
    else
    {
        
        return TEE_SUCCESS;
    }

}



TEE_Result g_SecStorTa_Dle(uint32_t paramTypes, TEE_Param params[4])
{
    TEE_Result l_ret = TEE_SUCCESS;
    CHAR* l_oldName = NULL;
    UINT32 l_oldLen = 0U;

    UNUSED(paramTypes);

    
    /** 1) Get the fd vaule of file which need be close */
    l_oldName = params[0].memref.buffer;
    l_oldLen = params[0].memref.size;

    TF("[DELETE] start to delete file: %s\n", l_oldName);
    l_ret = l_SecStorTa_OpenForSpe(l_oldName, l_oldLen);
    if (TEE_SUCCESS != l_ret)
    {        
        TF("[DELETE] open file fail\n");
        return TEE_EXEC_FAIL;
    }

    /** 3) Close the object handle of secure file */
    TEE_CloseAndDeletePersistentObject(g_FilesObj);

    if (TEE_SUCCESS != l_ret)
    {        
        TF("[DELETE] delete file fail\n");
        return TEE_EXEC_FAIL;
    }
    else
    {
        
        return TEE_SUCCESS;
    }
}













/**
 * @}
 */
