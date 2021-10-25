
#include "ican_port.h"
#include "string.h"
#include "ican.h"

#define LOG_TAG             "ican.port"
#include <ulog.h>

#define REIN_SEND_CNT					5			//重入队列最大次数

static ican_fd_msg_mq_t	ican_fd_msg_mq;

/** 
* can驱动接收的回调函数. 
* 无.
* @param[in]   dev:设备句柄，size：接收长度. 
* @param[out]  无.  
* @retval  rt_err_t 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size)
{
    /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&ican_fd_msg_mq.rx_sem);

    return RT_EOK;
}

/** 
* 协议层调用写入CAN发送队列. 
* 无.
* @param[in]   msg:can消息包. 
* @param[out]  无.  
* @retval  rt_err_t 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
rt_err_t can_write(struct rt_can_msg *msg)
{
	void *data;
	data = rt_malloc(sizeof(struct rt_can_msg));
	if( data ){
		ican_fd_port_msg_t port_msg;
		rt_memcpy(data,msg,sizeof(struct rt_can_msg));
		port_msg.data_ptr = data;
		port_msg.data_size = sizeof(struct rt_can_msg);
		port_msg.rein_send_cnt = 0;
		if( rt_mq_send(&ican_fd_msg_mq.out_mq, (void*)&port_msg, sizeof(ican_fd_port_msg_t)) != RT_EOK ){
			rt_free(data);
			LOG_W("the send queue is full, release the send memory");
			return RT_ERROR;
		}
		return RT_EOK;
	}
	else{
		LOG_W("can not malloc send memory");
		return RT_ERROR;
	}
}

/** 
* 从CAN发送队列读取CAN数据包写入CAN驱动. 
* 无.
* @param[in]   port:CAN驱动接口. 
* @param[out]  无.  
* @retval  rt_err_t 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
rt_err_t can_write_send( struct _ican_port *port )
{
	ican_fd_port_msg_t port_msg;

	/* 从消息队列中接收消息到 msg_ptr 中 */
	if (rt_mq_recv(&ican_fd_msg_mq.out_mq, (void*)&port_msg, sizeof(ican_fd_port_msg_t), 0 ) == RT_EOK){
		/* 成功接收到消息，进行相应的数据处理 */
		
//		rt_kprintf("get seg %d\r\n", ican_get_split_num(port_msg.data_ptr+8));
		
		rt_size_t size;
		size = rt_device_write(port->can_dev, 0, port_msg.data_ptr, port_msg.data_size);
		
		if (size == port_msg.data_size){
			
			rt_free(port_msg.data_ptr);
			return RT_EOK;
		}
		else{
			//调用驱动不成功，挂到队首下次再次发送
			if( port_msg.rein_send_cnt < REIN_SEND_CNT ){
				port_msg.rein_send_cnt++;
				rt_mq_urgent(&ican_fd_msg_mq.out_mq, (void*)&port_msg, sizeof(ican_fd_port_msg_t));
			}
			else{
				LOG_W("delete the send ican message");
				rt_free(port_msg.data_ptr);
			}
			return RT_ERROR;
		}
	}
	else{
		return RT_ERROR;
	}
}

/** 
* 从CAN接收队列读取CAN数据. 
* 无.
* @param[in]   data:返回数据指针. 
* @param[out]  无.  
* @retval  rt_err_t 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
rt_err_t can_read(struct rt_can_msg *data)
{
	ican_fd_port_msg_t port_msg;

	/* 从消息队列中接收消息到 msg_ptr 中 */
	if (rt_mq_recv(&ican_fd_msg_mq.in_mq, (void*)&port_msg, sizeof(ican_fd_port_msg_t), 0 ) == RT_EOK){
		/* 成功接收到消息，进行相应的数据处理 */
		rt_memcpy(data, port_msg.data_ptr,port_msg.data_size);
		
		rt_free(port_msg.data_ptr);
		
		return RT_EOK;
	}
	else{
		return RT_ERROR;
	}
}

/** 
* 从CAN驱动读出数据包写入CAN接收队列. 
* 无.
* @param[in]   port:CAN驱动接口. 
* @param[out]  无.  
* @retval  rt_err_t 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
rt_err_t can_read_recv( struct _ican_port *port )
{
	if( rt_sem_trytake(&ican_fd_msg_mq.rx_sem)==RT_EOK ){
	
		ican_fd_port_msg_t port_msg;
		rt_size_t size;
		struct rt_can_msg can_msg;
		
		/* hdr 值为 - 1，表示直接从 uselist 链表读取数据; hdr 为指定过滤表号，表示从hdrlist链表读取数据 */
        can_msg.hdr = -1;
		size = rt_device_read(port->can_dev, 0, &can_msg, sizeof(struct rt_can_msg));
		
		if( size ){
			
//			rt_kprintf("ID:0x%08x \n", can_msg.id);
//			for (uint8_t i = 0; i < can_msg.len; i++)
//			{
//				rt_kprintf("%02x ", can_msg.data[i]);
//			}

//			rt_kprintf("\n");
			
			port_msg.data_ptr = rt_malloc(sizeof(struct rt_can_msg));
			if( port_msg.data_ptr ){
				rt_memcpy(port_msg.data_ptr, &can_msg, sizeof(struct rt_can_msg));
				port_msg.data_size = sizeof(struct rt_can_msg);
				rt_mq_send(&ican_fd_msg_mq.in_mq, (void*)&port_msg, sizeof(ican_fd_port_msg_t));
				return RT_EOK;
			}
			else{
				LOG_W("can not malloc receive memory");
				return RT_ERROR;
			}
		}
		else{
			return RT_ERROR;
		}
	}
	else{
		return RT_ERROR;
	}
}

/** 
* CAN驱动初始化. 
* 无.
* @param[in]   port:CAN驱动接口，dev_name:驱动名称，Abaudrate:canfd 仲裁段波特率，Dbaudrate:数据段波特率，cfg:过滤表. 
* @param[out]  无.  
* @retval  rt_err_t 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
rt_err_t can_init(struct _ican_port *port, char *dev_name, uint32_t Abaudrate, uint32_t Dbaudrate, struct rt_can_filter_config *cfg)
{
	rt_device_t can_dev;
	
	strncpy(port->can_dev_name,dev_name,RT_NAME_MAX);
	
	can_dev = rt_device_find(port->can_dev_name);
    if (!can_dev){
		LOG_E("can not find can device");
        return RT_ERROR;
    }
	port->can_dev = can_dev;
	
	rt_err_t res;
	res = rt_device_open(can_dev, (RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX));
	
	if( res != RT_EOK ){
		LOG_E("can not open the can device");
		return RT_ERROR;
	}
	
#ifdef RT_CAN_USING_HDR
    /* 设置硬件过滤表 */
    res = rt_device_control(can_dev, RT_CAN_CMD_SET_FILTER, cfg);
    if( res != RT_EOK ){
		LOG_E("can not init the CAN FD HDR");
		return RT_ERROR;
	}
#endif
	
	struct rt_can_baudrate_config cfg_baud;
	cfg_baud.baudRateA = Abaudrate;
	cfg_baud.baudRateD = Dbaudrate;
	cfg_baud.mode = RT_CAN_MODE_CANFD;
	
	res = rt_device_control(can_dev, RT_CAN_CMD_SET_BAUD, &cfg_baud);
    if( res != RT_EOK ){
		return RT_ERROR;
	}
	
//	port->can_init = can_init;
//	port->can_write = can_write;
//	port->can_read = can_read;
//	port->can_read_recv = can_read_recv;
//	port->can_write_send = can_write_send;
	
	rt_sem_init(&ican_fd_msg_mq.rx_sem, "canrx_sem", 0, RT_IPC_FLAG_FIFO);
	
	rt_device_set_rx_indicate(port->can_dev, can_rx_call);
	
	rt_err_t result;
	ican_fd_msg_mq.in_pool = rt_malloc((RT_ALIGN(sizeof(struct _ican_fd_port_msg), RT_ALIGN_SIZE)+4)*ICAN_FD_SEG_MAX);
	if( ican_fd_msg_mq.in_pool ){

		/* 初始化消息队列 */
		result = rt_mq_init(&ican_fd_msg_mq.in_mq,
                        "in_mqt",
                        ican_fd_msg_mq.in_pool,             /* 内存池指向 msg_pool */
                        sizeof(struct _ican_fd_port_msg),   /* 每个消息的大小是 1 字节 */
                        (RT_ALIGN(sizeof(struct _ican_fd_port_msg), RT_ALIGN_SIZE)+4)*ICAN_FD_SEG_MAX,		  						/* 内存池的大小是 msg_pool 的大小 */
                        RT_IPC_FLAG_FIFO);					/* 如果有多个线程等待，按照先来先得到的方法分配消息 */
		
		if (result != RT_EOK){
			LOG_E("can not init the in queue");
			return RT_ERROR;
		}
	}
	else{
		LOG_E("can not malloc the in pool");
		return RT_ERROR;
	}
	
	ican_fd_msg_mq.out_pool = rt_malloc((RT_ALIGN(sizeof(struct _ican_fd_port_msg), RT_ALIGN_SIZE)+4)*ICAN_FD_SEG_MAX);
	if( ican_fd_msg_mq.out_pool ){
		
		/* 初始化消息队列 */
		result = rt_mq_init(&ican_fd_msg_mq.out_mq,
                        "out_mqt",
                        ican_fd_msg_mq.out_pool,             /* 内存池指向 msg_pool */
                        sizeof(struct _ican_fd_port_msg),	/* 每个消息的大小是 1 字节 */
                        (RT_ALIGN(sizeof(struct _ican_fd_port_msg), RT_ALIGN_SIZE)+4)*ICAN_FD_SEG_MAX,		  						/* 内存池的大小是 msg_pool 的大小 */
                        RT_IPC_FLAG_FIFO);					/* 如果有多个线程等待，按照先来先得到的方法分配消息 */
		
		if (result != RT_EOK){
			LOG_E("can not init the out queue");
			return RT_ERROR;
		}
	}
	else{
		LOG_E("can not malloc the out pool");
		return RT_ERROR;
	}
	
	return RT_EOK;
}

