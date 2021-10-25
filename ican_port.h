#ifndef __ICAN_PORT_H__
#define __ICAN_PORT_H__
#include "rtdevice.h"
#include "stdint.h"
#include "stdbool.h"

#define ICAN_FD_SEG_MAX					64

#pragma pack(1)
typedef struct _ican_fd_port_msg
{
    rt_uint8_t *data_ptr;    /* 数据块首地址 */
    rt_uint32_t data_size;   /* 数据块大小   */
	uint8_t		rein_send_cnt;
}ican_fd_port_msg_t;

typedef struct _ican_fd_msg_mq{
	
	uint8_t		*in_pool;
	struct rt_messagequeue in_mq;
	uint8_t 	*out_pool;
	struct rt_messagequeue out_mq;
	
	struct rt_semaphore rx_sem;     /* 用于接收消息的信号量 */
	
}ican_fd_msg_mq_t;

typedef struct _ican_port
{
	char can_dev_name[RT_NAME_MAX];
	rt_device_t can_dev;            /* CAN 设备句柄 */
	
//	rt_err_t (*can_read)(struct rt_can_msg *data);
//	rt_err_t (*can_write)(struct rt_can_msg *data);
//	rt_err_t (*can_init)(struct _ican_port *port, char *dev_name, uint16_t Abaudrate, uint16_t Dbaudrate);
//	rt_err_t (*can_write_send)( struct _ican_port *port );
//	rt_err_t (*can_read_recv)( struct _ican_port *port );
}ican_port_t;

#pragma pack()

rt_err_t can_read(struct rt_can_msg *data);
rt_err_t can_write(struct rt_can_msg *data);
rt_err_t can_init(struct _ican_port *port, char *dev_name, uint32_t Abaudrate, uint32_t Dbaudrate, struct rt_can_filter_config *cfg);
rt_err_t can_write_send( struct _ican_port *port );
rt_err_t can_read_recv( struct _ican_port *port );

#endif
