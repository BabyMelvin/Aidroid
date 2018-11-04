/*
 ****************************************************************************************
 *
 *               secStorTaType.h
 *
 * Filename      : secStorTaType.h
 * Programmer(s) : system BSP
 * Filename      : secStorTaType.h
 * Author        : Shuai Fengyun
 * Mail          : shuaifengyun@126.com
 * Create Time   : Mon 19 Jun 2017 10:43:47 AM CST
 ****************************************************************************************
 */

#ifndef MOUDLE_SST_TYPE_TA_H_
#define MOUDLE_SST_TYPE_TA_H_




/*
 *******************************************************************************
 *                                INCLUDE FILES
 *******************************************************************************
*/
#include "tee_internal_api.h"
#include "tee_api_defines.h"
#include "trace.h"
#include "tee_api_defines_extensions.h"
#include "string.h"




/*
 *******************************************************************************
 *                  MACRO DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/
/* Define the return status of each function */
#define   FAIL     -1            /* Return value when operation fail */
#define   OK        0            /* Return value when operation OK */


#define   TEE_ALG_INVALID 0x0000FFFF
#define TEE_EXEC_FAIL   0x0000FFFF
    
    
    
/* Define the debug flag */
#define DEBUG
#define TF    MSG_RAW
//#define TF    ta_debug
    
#define UNUSED(x) (void)(x)





/*
 *******************************************************************************
 *                STRUCTRUE DEFINITION USED ONLY BY THIS MODULE
 *******************************************************************************
*/
/* Define the type of variable */
typedef unsigned char  UINT8;    /**< Typedef for 8bits unsigned integer  */
typedef unsigned short UINT16;   /**< Typedef for 16bits unsigned integer */
typedef uint32_t       UINT32;   /**< Typedef for 32bits unsigned integer */
typedef signed char    INT8;     /**< Typedef for 8bits signed integer    */
typedef signed short   INT16;    /**< Typedef for 16bits signed integer   */
typedef signed int     INT32;    /**< Typedef for 32bits signed integer   */
typedef char           CHAR;     /**< Typedef for char                    */

typedef uint32_t       TEE_CRYPTO_ALGORITHM_ID;





/*
 *******************************************************************************
 *                      VARIABLES SUPPLIED BY THIS MODULE
 *******************************************************************************
*/





/*
 *******************************************************************************
 *                      FUNCTIONS SUPPLIED BY THIS MODULE
 *******************************************************************************
*/






















#endif  /* MOUDLE_NAME_H*/
