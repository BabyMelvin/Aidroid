/****************************************************************************************/
/*                          COPYRIGHT INFORMATION                                       */
/*    All rights reserved:shuaifengyun@126.com.                                         */
/****************************************************************************************/
/*
 ****************************************************************************************
 *
 *               CryptoVerifyTaOther.c
 *
 * Filename      : CryptoVerifyTaOther.c
 * Author        : Shuai Fengyun
 * Mail          : shuai.fengyun@126.com
 * Create Time   : Fri 30 Oct 2015 12:41:43 AM EDT
 ****************************************************************************************
 */

#define MOUDLE_CRYPTO_TA_OTHER_C_

/** @defgroup MODULE_NAME_INFOR
* @{
*/

/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "CryptoVerifyTaOther.h"
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
void g_CryptoTaOther_Random(UINT32 len, CHAR* output);
void g_CryptoTaOther_base64(CHAR* input, UINT32 lenIn, CHAR* output, UINT32 lenOut, UINT32 oper);





/*
 *******************************************************************************
 *                          VARIABLES USED ONLY BY THIS MODULE
 *******************************************************************************
*/
static const unsigned char map[256] = 
{
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
     52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
    255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
      7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
     19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
    255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
     37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
     49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255 
};

static const char *codes = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


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
void g_CryptoTaOther_Random(UINT32 len, CHAR* output)
{
    TF("Entry random\n");
    TEE_GenerateRandom(output, len);
}

/** @ingroup MOUDLE_NAME_C_
 *- #Description  This function for handle command.
 * @param   pMsg           [IN] The received request message
 *                               - Type: MBX_Msg *
 *                               - Range: N/A.
 *
 * @return     int    return the length of decode data
 * @retval     void
 *
 *
 */
static int l_DeviceIdTaBase64_encode(const CHAR* in,  UINT32 inlen, CHAR* out, UINT32* outlen)
{
    unsigned long i, len2, leven;
    char *p;
    int l_Ret = FAIL;

    /* valid output size ? */
    len2 = 4 * ((inlen + 2) / 3);
    if (*outlen < len2 + 1) 
    {
        *outlen = len2 + 1;
        TF("Judge output size faile\n");
        return l_Ret;
    }
    p = out;
    leven = 3*(inlen / 3);
    for (i = 0; i < leven; i += 3) 
    {
        *p++ = codes[(in[0] >> 2) & 0x3F];
        *p++ = codes[(((in[0] & 3) << 4) + (in[1] >> 4)) & 0x3F];
        *p++ = codes[(((in[1] & 0xf) << 2) + (in[2] >> 6)) & 0x3F];
        *p++ = codes[in[2] & 0x3F];
        in += 3;
    }
    /* Pad it if necessary...  */
    if (i < inlen) 
    {
        unsigned a = in[0];
        unsigned b = (i+1 < inlen) ? in[1] : 0;

        *p++ = codes[(a >> 2) & 0x3F];
        *p++ = codes[(((a & 3) << 4) + (b >> 4)) & 0x3F];
        *p++ = (i+1 < inlen) ? codes[(((b & 0xf) << 2)) & 0x3F] : '=';
        *p++ = '=';
    }

    /* append a NULL byte */
    *p = '\0';

    /* return ok */
    *outlen = p - out;
    l_Ret = *outlen;
    return l_Ret;
}


static int l_DeviceIdTaBase64_decode(const CHAR* in,  UINT32 inlen, CHAR* out, UINT32* outlen)
{
    unsigned long t, x, y, z;
    unsigned char c;
    int           g;
    int l_Ret = FAIL;


    g = 3;
    for (x = y = z = t = 0; x < inlen; x++) 
    {
        c = map[in[x]&0xFF];
        if (c == 255) continue;
        /* the final = symbols are read and used to trim the remaining bytes */
        if (c == 254) 
        { 
            c = 0; 
            /* prevent g < 0 which would potentially allow an overflow later */
            if (--g < 0) 
            {
                return l_Ret;
            }
        } 
        else if (g != 3) 
        {
            /* we only allow = to be at the end */
            return l_Ret;
        }

        t = (t<<6)|c;

        if (++y == 4) 
        {
            if (z + g > *outlen) 
            { 
                return l_Ret; 
            }
            out[z++] = (unsigned char)((t>>16)&255);
            if (g > 1) out[z++] = (unsigned char)((t>>8)&255);
            if (g > 2) out[z++] = (unsigned char)(t&255);
            y = t = 0;
        }
    }
    if (y != 0) 
    {
        return l_Ret;
    }
    *outlen = z;
    l_Ret = *outlen;
    return l_Ret;
}





void g_CryptoTaOther_base64(CHAR* input, UINT32 lenIn, CHAR* output, UINT32 lenOut, UINT32 oper)
{
    int l_Ret = -1;
    /* 1U is for enc operation ,2U is for dec operation */
    if(1U == oper)
    {
        TF("Do encode operation!\n");
        l_Ret = l_DeviceIdTaBase64_encode(input, lenIn, output, &lenOut);
        TF("The return value is: %d\n", l_Ret);
    }
    else if(2U == oper)
    {
        TF("Do decode operation!\n");
        l_Ret = l_DeviceIdTaBase64_decode(input, lenIn, output, &lenOut);
        TF("The return value is: %d\n", l_Ret);
    }
    else
    {
    }
 
}








/**
 * @}
 */
