#ifndef TA_MY_TEST_H
#define TA_MY_TEST_H

/* This UUID is generated with uuidgen
   the ITU-T UUID generator at http://www.itu.int/ITU-T/asn1/uuid.html */
/* 9269fadd-99d5-4afb-a1dc-ee3e9c61b04c  */
#define TA_MY_TEST_UUID { 0x9269fadd, 0x99d5, 0x4afb, \
		{ 0xa1, 0xdc, 0xee, 0x3e, 0x9c, 0x61, 0xb0, 0x4c} }

/* The Trusted Application Function ID(s) implemented in this TA */
#define TA_MY_TEST_CMD_INC_VALUE	0
#define TA_MY_TEST_CMD_HASH	        1
#define TA_MY_TEST_CMD_RANDOM	    2


#define FAIL -1
#define OK 0
#define TEE_ALG_INVALID     0xFFFFFFFFU


#endif /*TA_HELLO_WORLD_H*/
