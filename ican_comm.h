#ifndef __COMM_H__
#define __COMM_H__

#include "ican_port.h"
#include "ican.h"
#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

//#define ICAN_MQ_NUM						10
#define ICAN_MSG_MAX					10				//最大接收的ICAN消息数
#define ICAN_HEADER_LEN					2				//ICAN协议占用数据区长度
#define ICAN_CFG_HEADER_LEN				3				
#define ICAN_MAX_PACK					3968			//最大ICAN数据包
#define ICAN_SEG_TIMEOUT				100				//分包接收超时时间
#define REIN_THREAD_CNT					10				//最大提交给线程的次数
#define ICAN_BROADCAST_ADDR				0xff			//广播地址

//icanЭөؖ׎ԫˤҪ־۪
#define ICAN_NO_SPLIT_SEG				0x00			
#define ICAN_SPLIT_SEG_FIRST			0x01
#define ICAN_SPLIT_SEG_MID				0x02
#define ICAN_SPLIT_SEG_LAST				0x03

#define ICAN_MSG_OK						0x00
#define ICAN_MSG_EXCEPTION				0x01
#define ICAN_MSG_TIMEOUT				0x02

typedef struct _ican_fd_msg
{
	ican_fd_id_t	id;
	uint8_t			*data_ptr;
	uint16_t		data_len;
	uint8_t			rein_cnt;
}ican_fd_msg_t;

typedef struct _ican_sub_recv
{	
	ican_fd_id_t	id;
	uint16_t		timeout;
	uint8_t 		recv_comp;
	
	uint8_t 		cur_flag;
	uint8_t 		old_flag;
	uint8_t 		cur_num;
	uint8_t 		old_num;
	uint8_t 		old_src_id;
	uint8_t 		cur_src_id;
	uint16_t 		offset;
	uint16_t 		length;
	uint8_t 		*buff;
}ican_sub_recv_t;

typedef struct _ican_comm
{
	ican_sub_recv_t	sub_recv[ICAN_MSG_MAX];
	ican_port_t		can_port;
	struct rt_messagequeue ican_mq;
	uint8_t			msg_pool[(RT_ALIGN(sizeof(ican_fd_msg_t), RT_ALIGN_SIZE)+4)*ICAN_MSG_MAX];
}ican_comm_t;

extern int ican_read_msg(ican_fd_id_t *id, ican_fd_id_t *mask, uint8_t *data, uint16_t size, rt_int32_t timeout, uint16_t *msg_size);
extern INT8U ican_send_msg(ican_fd_frame_send_t * ican_pack);
extern rt_err_t ican_comm_init(void *parameter);
#endif