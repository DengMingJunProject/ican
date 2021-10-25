#ifndef __ICAN_CFG_H__
#define __ICAN_CFG_H__

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)<In Cotex-M3>
*********************************************************************************************************
*/

typedef unsigned char  	BOOLEAN;
typedef unsigned char  	INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  	INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short	INT16U;					  /* Unsigned  16 bit quantity							*/
typedef signed short 	INT16S;					  /* Signed    16 bit quantity							*/
typedef unsigned long  	INT32U;                   /* Unsigned  32 bit quantity                          */
typedef signed   long  	INT32S;                   /* Signed    32 bit quantity                          */
typedef float          	FP32;                     /* Single precision floating point                    */
typedef double         	FP64;                     /* Double precision floating point                    */

#define DEV_IS_MATSER			1

/*ican io each source length define 
di is digital input the max is 256X8 = 2048bit 
do is digital output the max is 256X8 = 2048bit
ai is ad convert data,according to the conversion accuracy if 8bit accuracy the max is 256 byte if 12bit or 16bit the max is 128 byte
ao is da convert data,as the ad convert ,if 8bit accuracy the max length is 256 byte,if 12bit or 16bit accuracy the max length is 128byte
serial0 is serial port0 data buff length ,the max leng is 256 byte,default is 0byte
serial1 is the same as serial0
user is the user define data length depends users define .default is 0byte
*/
#define ICAN_DI_LEN				6 			/*8bit digital input*/
#define ICAN_DO_LEN				6 			/*8bit digital output*/
#define ICAN_AI_LEN				6 			/*8 ad channel if conversion accuracy is 8bit then the length is 8byte else if 12bit or 16bit conversion cccuracy the length is 16byte*/
#define ICAN_AO_LEN				6 			/*8 da channel as the ad convert if conversion accuracy is 8bit then the length is 8byte,if 12bit or 16bit the length is 16byte*/
#define ICAN_SER0_LEN			20 			/*serial port0 data length*/
#define ICAN_SER1_LEN			34 			/*serial port1 data length*/
#define ICAN_USER_LEN			34			/*user define data length,default is 0*/

//ican设备固件信息
#define VENDOR_ID				0x4347
#define PRODUCT_TYPE 			0x00
#define PRODUCT_CODE			0x00
#define HARDWARE_VERSION		0x01
#define FIRMWARE_VERSION 		0x01
#define SERIAL_NUMBER			0xa55aa55a

//ican设备通讯信息
#define RATE_500K				0x00
#define RATE_250K				0x01
#define RATE_125K				0x02
#define RATE_100K				0x03
#define RATE_10K				0x04
#define RATE_NOT_EFFECT			0xff

#define COSTYPE_DISABLE			0x00
#define COSTYPE_ENABLE			0x01

#if (DEV_IS_MATSER > 0)
#define DEV_MAC_ID 				0x00
#define BAUD_RATE				RATE_NOT_EFFECT
#define USER_BAUD_RATE			RATE_250K
#define CYCLIC_PARAM			0x00
#define CYCLIC_MASTER			0x01
#define COS_TYPE				COSTYPE_DISABLE
#define MASTER_MAC_ID			0x00
#else
#define DEV_MAC_ID 				0x00
#define BAUD_RATE				RATE_NOT_EFFECT
#define USER_BAUD_RATE			RATE_250K
#define CYCLIC_PARAM			0x00
#define CYCLIC_MASTER			0x01
#define COS_TYPE				COSTYPE_DISABLE
#define MASTER_MAC_ID			0xff
#endif

//ican协议工作缓存大小
#define MAX_DATA_BUFF			64

#endif
