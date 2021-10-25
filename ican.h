#ifndef __ICAN_H__
#define __ICAN_H__

#include "ican_cfg.h"
#include "stdint.h"
#include "stdbool.h"

/*ican protocol source define 
0x00 to 0x7f is used as ican io source
0x80 to 0xff is used as ican config source
ican io source include :
di source			do source 			ai source 
ao source 			serical0 source		serical1 source 
user_def source 	reserve
ican config source include :
device identify source			communicate parameters source
io parameters source			io config source		
*/

#define ICAN_IO_SRC		1
#define ICAN_CFG_SRC	2


/*ican io source define*/
#define ICAN_SRC_DI									0x00
#define ICAN_SRC_DO									0x01
#define ICAN_SRC_AI									0x02
#define ICAN_SRC_AO									0x03
#define ICAN_SRC_SERIAL0							0x04
#define ICAN_SRC_SERIAL1							0x05
#define ICAN_SRC_RESERVE							0x06
#define ICAN_SRC_USERDEF							0x07
#define ICAN_SRC_THROUGH							0x08
#define ICAN_SRC_EXCEPTION							0x0f
/*ican config source define*/
#define ICAN_CFG_DEVICE_INFO						0x80
#define ICAN_CFG_COMMUNICATE						0x81
#define ICAN_CFG_IO_PARAM							0x82
#define ICAN_CFG_IO_CFG								0x83
#define ICAN_CFG_IO_RESERVE							0x84
#define ICAN_SRC_UNDEFINED							0xff
#define ICAN_ESTABLISH_CONNECT						0xf7

#define ICAN_VENDOR_ID								0x00
#define ICAN_PRODUCT_TYPE							0x01
#define ICAN_PRODUCT_CODE							0x02
#define ICAN_HARDWARE_VERSION						0x03
#define ICAN_FIRMWARE_VERSION						0x04
#define ICAN_SERIAL_NUMBER							0x05

#define ICAN_MAC_ID									0x06
#define ICAN_BAUDRATE								0x07
#define ICAN_USER_BAUDRATE							0x08
#define ICAN_CYCLIC_PARAM							0x09
#define ICAN_CYCLIC_MASTER							0x0a
#define ICAN_COS_TYPE								0x0b
#define ICAN_MASTER_MAC_ID							0x0c

#define ICAN_IO_PARAM								0x0d
#define ICAN_IO_CFG									0x0e
#define ICAN_IO_RESERVE								0x0f

/*ican config source sub offset define*/
#define OFFSET_VENDOR_ID							0x00
#define OFFSET_PRODUCT_TYPE							0x02
#define OFFSET_PRODUCT_CODE							0x04
#define OFFSET_HARDWARE_VERSION						0x06
#define OFFSET_FIRMWARE_VERSION						0x08
#define OFFSET_SERIAL_NUMBER						0x0a

#define OFFSET_MAC_ID								0x00
#define OFFSET_BAUDRATE								0x01
#define OFFSET_USER_BAUDRATE						0x02
#define OFFSET_CYCLIC_PARAM							0x06
#define OFFSET_CYCLIC_MASTER						0x07
#define OFFSET_COS_TYPE								0x08
#define OFFSET_MASTER_MAC_ID						0x09

#define OFFSET_DI_PARAM								0x00
#define OFFSET_DO_PARAM								0x01
#define OFFSET_AI_PARAM								0x02
#define OFFSET_AO_PARAM								0x03
#define OFFSET_SERIAL0_PARAM						0x04
#define OFFSET_SERIAL1_PARAM						0x05
#define OFFSET_USERDEF_PARAM						0x06

#define OFFSET_DI									0x00
#define OFFSET_DO									0x01
#define OFFSET_AI									0x02
#define OFFSET_AO									0x03
#define OFFSET_SERIAL0								0x04
#define OFFSET_SERIAL1								0x06
#define OFFSET_USERDEF								0x08
/*ican function code define */	
#define ICAN_FUC_Reserve							0x00				
#define ICAN_FUC_WRITE								0x01
#define ICAN_FUC_READ								0x02
#define ICAN_FUC_EVE_TRAGER							0x03
#define ICAN_FUC_EST_CONNECT						0x04
#define ICAN_FUC_DEL_CONNECT						0x05
#define ICAN_FUC_DEV_RESET							0x06
#define ICAN_FUC_MAC_CHECK							0x07
#define ICAN_FUC_SENDTO								0x08
#define ICAN_FUC_ERROR								0x0f
/*ican error code define */
#define ICAN_OK										0x00
#define ICAN_ERR_FUNC_ID							0x01
#define ICAN_ERR_SOURCE_ID							0x02
#define ICAN_ERR_COMMAND							0x03
#define ICAN_ERR_PARAM								0x04
#define ICAN_ERR_COMMUNICATE						0x05
#define ICAN_ERR_OPERATE							0x06
#define ICAN_ERR_TRANS								0x07
#define ICAN_ERR_MAC_UNMATCH						0x08
#define ICAN_ERR_TIME_OUT							0x09
#define ICAN_ERR_BUFF_OVERFLOW						0x0a
#define ICAN_ERR_LEN_ZERO							0x0b
#define ICAN_ERR_NODE_BUSY							0x0c
#define ICAN_ERR_DEL_NODE							0x0d
#define ICAN_ERR_NODE_EXIST							0x0e
#define ICAN_ERR_MAC_EXIST							0x0f
#define ICAN_SETUP_CONNECT							0x10
#define ICAN_ERR_ADDR								0xff

//自定义ICAN异常码
#define ICAN_ERR_SEND								0x80
#define ICAN_ERR_MEM								0x81
#define ICAN_ERR_RECV								0x82
#define ICAN_ERR_SLAVE								0x83
#define ICAN_ERR_UNDEF								0xff

#define CAN_DATA_SIZE								64

#define ICAN_ID_ITEM_INIT(dest,src,ack,func,source) \
     (src<<21|dest<<13|ack<<12|func<<8|source)

#pragma pack(1)
typedef struct _ican_src_node
{
	uint8_t			type;
	uint8_t			sub;
	uint8_t			start_addr;
	uint8_t			end_addr;
	uint8_t			declare;
}ican_src_node_t;

/*ican frame define*/
typedef struct _ican_fd_id
{
	INT32U  
	source_id 	: 8,
	func_id		: 4,
	ack 		: 1,
	dest_mac_id : 8,
	src_mac_id  : 8,
	rev			: 3;
}ican_fd_id_t;

typedef struct _ican_fd_frame
{
	ican_fd_id_t	id;
	INT8U	dlc;
	INT8U	frame_data[CAN_DATA_SIZE];
}ican_fd_frame_t;

typedef struct _ican_fd_frame_send
{
	ican_fd_id_t	id;
	INT8U			dlc;
	uint8_t			offset;
	uint8_t			src_cfg;
	uint16_t		len;
	INT8U			*frame_data_ptr;
}ican_fd_frame_send_t;

typedef struct _ican_fd_io_src
{
#if ICAN_DI_LEN
	INT8U di_data[ICAN_DI_LEN];
#endif
#if ICAN_DO_LEN
	INT8U do_data[ICAN_DO_LEN];
#endif
#if ICAN_AI_LEN
	INT8U ai_data[ICAN_AI_LEN];
#endif
#if ICAN_AO_LEN
	INT8U ao_data[ICAN_AO_LEN];
#endif
#if ICAN_SER0_LEN
	INT8U serial0_data[ICAN_SER0_LEN];
#endif
#if ICAN_SER1_LEN
	INT8U serial1_data[ICAN_SER1_LEN];
#endif
#if ICAN_USER_LEN
	INT8U userdef_data[ICAN_USER_LEN];
#endif
	void * arg;
}ican_fd_io_src_t;

typedef struct _device_info
{
	INT16U vendor_id;
	INT16U product_type;
	INT16U product_code;
	INT16U hardware_version;
	INT16U firmware_version;
	INT32U serial_number;
}ican_fd_dev_info_t;

typedef struct _communicate_info
{
	INT8U  	dev_mac_id;
	INT8U  	baud_rate;
	INT32U  user_baud_rate;
	INT8U  	cyclic_param;
	INT8U  	cyclic_master;
	INT8U  	cos_type;
	INT8U  	master_mac_id;
}ican_fd_com_info_t;

typedef struct _io_param
{
	INT8U di_length;
	INT8U do_length;
	INT8U ai_length;
	INT8U ao_length;
	INT8U serial0_length;
	INT8U serial1_length;
	INT8U userdef_length;
}ican_fd_io_param_t;

#pragma pack()

INT8U ican_get_src_macid(ican_fd_frame_t * pframe);
void ican_set_src_macid(ican_fd_frame_t * pframe, INT8U src_macid);
INT8U ican_get_dest_macid(ican_fd_frame_t * pframe);
void ican_set_dest_macid(ican_fd_frame_t * pframe, INT8U dest_macid);
INT8U ican_get_func_id(ican_fd_frame_t * pframe);
void ican_set_func_id(ican_fd_frame_t * pframe, INT8U func_id);
INT8U ican_get_source_id(ican_fd_frame_t * pframe);
void ican_set_source_id(ican_fd_frame_t * pframe, INT8U source_id);
INT8U ican_is_need_ack(ican_fd_frame_t * pframe);
void ican_set_ack(ican_fd_frame_t * pframe, INT8U ack);
INT8U ican_get_split_flag(ican_fd_frame_t * pframe);
void ican_set_split_flag(ican_fd_frame_t * pframe, INT8U split_flag);
INT8U ican_get_split_num(ican_fd_frame_t * pframe);
void ican_set_split_num(ican_fd_frame_t * pframe, INT8U split_num);
void ican_clear_split_info(ican_fd_frame_t * pframe);
INT8U ican_get_offset(ican_fd_frame_t * pframe);
void ican_set_offset(ican_fd_frame_t * pframe, INT8U offset);
INT8U ican_get_length(ican_fd_frame_t * pframe);
void ican_set_length(ican_fd_frame_t * pframe, INT8U length);

INT8U ican_get_src_id(uint8_t addr,uint8_t *len);
INT8U ican_get_src_addr(uint8_t src_id, uint8_t offset);
INT8U ican_get_src_type(uint8_t src_id);
#endif
