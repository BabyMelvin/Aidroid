/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyTaPbkdf2.c
 *
 * Filename      : CryptoVerifyTaPbkdf2.c
 * Author        : Shuai Fengyun
 * Mail          : shuai.fengyun@126.com
 * Create Time   : Fri 30 Oct 2015 12:40:45 AM EDT
 ****************************************************************************************
 */

#define MOUDLE_CRYPTO_TA_PBKDF_C_

/** @defgroup MODULE_NAME_INFOR
* @{
*/

/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "CryptoVerifyTaPbkdf2.h"
#include "CryptoVerifyTaHash.h"




/*
 *******************************************************************************
 *                         FUNCTIONS SUPPLIED BY THIS MODULE
 *******************************************************************************
*/
void g_CryptoTaPbkdf_PBKDF2(CHAR P[],int Plen, CHAR S[], int Slen, int c,int dkLen, CHAR* output);





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
static void l_CryptoTaPbkdf_HMAC(CHAR* text, UINT32 tlen, CHAR* key, UINT32 klen, CHAR* output)
{
    CHAR key_append[64] = {0};
    CHAR stringText[256] = {0};
    CHAR x_Hash[20] = {0};
    UINT32 stringXLen = 64U;
    UINT32 stringYLen = 64U;
    CHAR y_Hash[20] = {0};
    CHAR ipad = 0x36;
    CHAR opad = 0x5c;
    UINT32 index = 0U;
    CHAR X[64] = {0};
    CHAR Y[64] ={0};
    UINT32 l_HashLen = 20U;


    /**1) Copy input key data into key append */
    if(klen > 64U)
    {
        TEE_MemMove(key_append, key, 64U);
    }
    else
    {
        TEE_MemMove(key_append, key, klen);
    }
    

    /**2) Do xor operation */
    for(index = 0U; index < 64U; index++)
    {
        X[index] = key_append[index]^ipad;   
        Y[index] = key_append[index]^opad;
    }

    /**3) Put the X data and text data into stringText */
    for(index = 0U; index < 64U; index++)
    {
        stringText[index] = X[index];
    }

    for(index = 0U; index < tlen; index++)
    {
        stringText[64 + index] = text[index];
        stringXLen++;
    }

    /**4) Do the hash operation for stringText */
    g_CryptoTaHash_sha(EN_OP_SHA1, stringText, stringXLen, x_Hash, &l_HashLen);

    /**5)  */
    for(index = 0U; index < 64U; index++)
    {
        stringText[index] = Y[index];
    }

    for(index = 0U; index < 20; index++)
    {
        stringText[64 + index] = x_Hash[index];
        stringYLen++;
    }

    /**6) Do hash operation */
    g_CryptoTaHash_sha(EN_OP_SHA1, stringText, stringYLen, y_Hash, &l_HashLen);

    /**7) Copy result to output */
    TEE_MemMove(output, y_Hash, 20U);
}








/** @ingroup MODULE_MANAGE_KEY_TA
 *- #Description  Support the pbkdf2 Algorithm for getting the secret key
 * @param   P         [IN] password string
 *                               - Type: char
 *                               - Range: N/A.
 * @param   Plen      [IN] The length of password
 *                               - Type: MBX_Msg *
 *                               - Range: N/A.
 * @param   S         [IN] The received request message
 *                               - Type: MBX_Msg *
 *                               - Range: N/A.
 * @param   Slen      [IN] The received request message
 *                               - Type: MBX_Msg *
 *                               - Range: N/A.
 * @param   c         [IN] The received request message
 *                               - Type: MBX_Msg *
 *                               - Range: N/A.
 * @param   dkLen     [IN] The received request message
 *                               - Type: MBX_Msg *
 *                               - Range: N/A.
 *
 * @return     int
 
 * @retval      OK: If got the right share memory address,return this value
 * @retval    FAIL: If got the wrong share memory address,return this value
 *
 *
 */
void g_CryptoTaPbkdf_PBKDF2(CHAR P[],int Plen, CHAR S[], int Slen, int c,int dkLen, CHAR* output)
{
    CHAR tmp_hmac[20] = {0};
    CHAR resultBuf[512] = {0};
    CHAR U_tmp[128] = {0};
    UINT32 uLen = 0U;
    UINT32 l_Lnum = 0U;
    //UINT32 l_Rnum = 0U;
    UINT32 indexI = 0U;
    UINT32 indexJ = 0U;
    UINT32 index =0U;
    TF("Pwd:%s\n", P);
    TF("Pwd len:%d\n", Plen);
    TF("Salt:%s\n", S);
    TF("Salt len:%d\n", Slen);
    TF("dkLen:%d\n", dkLen);
    TF("C:%d\n", c);


    /**1) Do the cail operation for getting loop num of l_Lnum */
    if((0 == dkLen%20) &&(20 < dkLen))
    {
        l_Lnum = (UINT32)(dkLen/20);
    }
    else
    {
        l_Lnum = (UINT32)((dkLen/20U))+1U; // hLen-octets 32/20 = 2 always 
    }

    /**2) Get the number of octest */
    //l_Rnum = (UINT32)dkLen -(l_Lnum-1)*20U; // number of octets in the last block (32-20) = 12
    
    for(indexI = 1U; indexI <= l_Lnum; indexI++)
    {           
        /**3) Copyt salt into tmp buffer for do the first HMAC operation */ 
        TEE_MemFill(U_tmp, 0, 128);
        TEE_MemMove(U_tmp, S, Slen);
        U_tmp[Slen + 3] = indexI;
        uLen = Slen + 4;

        
       // TF("{~~%d~~} First init T data just like follow:", indexI);
        l_CryptoTaPbkdf_HMAC(U_tmp, uLen, P, Plen, tmp_hmac);
        //g_TA_Printf(tmp_hmac, 20);
       // TF("\n");
        
        TEE_MemFill(U_tmp, 0, 128U);
        TEE_MemMove(U_tmp, tmp_hmac, 20U);
        uLen = 20U;
        
        for(indexJ = 1U; indexJ < (UINT32)c; indexJ++)         // number of iterations (4096)      
        {
          //  TF("[%d] HMAC output just like follow:", indexJ);
            l_CryptoTaPbkdf_HMAC(tmp_hmac, uLen, P, Plen, tmp_hmac);
           // g_TA_Printf(tmp_hmac, 20);
          //  TF("\n");

            for(index = 0U; index < 20U; index++)
            {
                U_tmp[index] = U_tmp[index] ^ tmp_hmac[index];
            }
        }               

        for(index = 0U; index < 20U; index++)
        {
            resultBuf[(indexI - 1U) * 20 + index] = U_tmp[index];
        }
    } 

    /**4) Copy the result data into output buffer */
    TEE_MemMove(output, resultBuf, dkLen);
}














/**
 * @}
 */
