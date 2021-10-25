#ifndef __ICAN_MASTER_H__
#define __ICAN_MASTER_H__

#include "ican.h"
#include "ican_comm.h"
//#include "bxcan.h"
#include "rtthread.h"
#include "rtdevice.h"
#include "ican_port.h"

#define ICAN_SLAVE_NUM					10

//ican协议状态机宏
#define ICAN_IDLE_STATUS 				0x00
#define ICAN_CHECK_STATUS				0x01
#define ICAN_RECV_STATUS 				0x02
#define ICAN_SEND_STATUS 				0x03
#define ICAN_PARSE_STATUS 				0x04
#define ICAN_ERROR_STATUS 				0x05

//ican协议连接状态
#define ICAN_IN_CONNECT					0x01
#define ICAN_IN_TIME_OUT				0x02
#define ICAN_IN_DISCONNECT				0x03

#define ICAN_MASTER_CHANNEL				BXCAN_CHANNEL1

#pragma pack(1) 
typedef struct _slave_src
{
	uint8_t			slave_id;
	INT8U 			slave_status;
	uint8_t			poll_time;

	ican_fd_io_src_t 	io_src;
	ican_fd_dev_info_t	dev_inf;
	ican_fd_com_info_t	com_cfg;
	ican_fd_io_param_t	io_param;
}slave_src_t;

typedef struct _ican_master
{
	uint8_t 		  	dev_addr;
	uint8_t 		  	dev_status;	
	uint8_t 		  	slave_setup_check;
	uint8_t				mac_conflict;

	ican_fd_dev_info_t 	dev_inf;
	ican_fd_com_info_t 	com_cfg;
	slave_src_t	  	slave_src_list[ICAN_SLAVE_NUM];
	
	ican_comm_t			ican_comm;
	
}ican_master;

#pragma pack()

extern ican_master * pimaster;

//INT8U ican_master_read(INT8U slave_id, INT8U src_id, INT8U offset, INT16U len, INT8U * buff);
//INT8U ican_master_write(INT8U slave_id, INT8U src_id, INT8U offset, INT16U len, INT8U * buff);
//INT8U ican_master_event_trager(INT8U slave_id, INT8U src_id, INT8U offset, INT8U len, INT8U * buff);
//INT8U ican_master_establish_connect(INT8U slave_id);
//INT8U ican_master_delete_connect(INT8U slave_id);
//INT8U ican_master_device_reset(INT8U slave_id);
//INT8U ican_master_mac_check(INT8U slave_id, INT8U src_id, INT8U offset, INT8U len, INT8U * buff);
//int ican_master_init(char *dev_name, uint32_t baudrate);

#endif
