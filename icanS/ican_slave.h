#ifndef __ICAN_SLAVE_H__
#define __ICAN_SLAVE_H__

#include "ican.h"
#include "ican_comm.h"

//ican协议默认从机地址
#define SLAVE_DEV_ADDR					1
//ican协议分段传输标志宏
#define ICAN_NO_SPLIT_SEG				0x00			
#define ICAN_SPLIT_SEG_FIRST			0x01
#define ICAN_SPLIT_SEG_MID				0x02
#define ICAN_SPLIT_SEG_LAST				0x03

//ican协议状态机宏
#define ICAN_RECV_STATUS 				0x01
#define ICAN_SEND_STATUS 				0x02
#define ICAN_PARSE_STATUS 				0x03

//ican协议连接状态
#define ICAN_IN_CONNECT					0x01
#define ICAN_IN_TIME_OUT				0x02
#define ICAN_IN_DISCONNECT				0x03

#define ICAN_SLAVE_CHANNEL				BXCAN_CHANNEL2

//ican协议数据结构
typedef struct _ican_slave
{
	INT8U 			slave_status;

	ican_fd_io_src_t 	io_src;
	ican_fd_dev_info_t	dev_inf;
	ican_fd_com_info_t	com_cfg;
	ican_fd_io_param_t	io_param;
	
	uint8_t				mac_conflict;
	uint16_t			comm_timeout;
	uint16_t			cyclic_time;
	
	ican_comm_t		ican_comm;
}ican_slave_t;

extern ican_slave_t * pican_slave;

INT16U ican_get_vendor_id(void);
INT16U ican_get_product_type(void);
INT16U ican_get_product_code(void);
INT16U ican_get_hardware_version(void);
INT16U ican_get_firmware_version(void);
INT32U ican_get_serial_number(void);
INT8U ican_get_dev_mac_id(void);
void ican_set_dev_mac_id(INT8U id);
INT8U ican_get_baud_rate(void);
void ican_set_baud_rate(INT8U rate);
INT32U ican_get_user_baud_rate(void);
void ican_set_user_baud_rate(INT32U usr_rate);
INT8U ican_get_cyclic_param(void);
void ican_set_cyclic_param(INT8U time);
INT8U ican_get_cyclic_master(void);
void ican_set_cyclic_master(INT8U time);
INT8U ican_get_cos_type(void);
void ican_set_cos_type(INT8U type);
INT8U ican_get_master_mac_id(void);
void ican_set_master_mac_id(INT8U id);

#endif
