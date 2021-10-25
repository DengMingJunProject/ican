/** 
* @file         ican_master.c 
* @brief        ICAN主站命令处理. 
* @details  	管理从站连接，轮询从站的数据. 
* @author       ken deng 
* @date     	2020-05-26 
* @version  	A001 
* @par Copyright (c):  
* 
* @par History:          
*   version: ken deng, 2020-05-26, 建立\n 
*/ 
#include "ican.h"
#include "ican_master.h"
#include "ican_convert.h"
#include "rtthread.h"
#include "string.h"
#include "ican_port.h"

#define LOG_TAG             "ican.master"
#include <ulog.h>
	
ican_master * pimaster;
static ican_master imaster;

/** 
* 获取ICAN指定ID的资源地址. 
* 无.
* @param[in]   id:从站ID.
* @param[out]  无.  
* @retval  地址. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
static void* ican_get_slave_handle(uint8_t id)
{
	slave_src_t *p = imaster.slave_src_list;
	uint8_t i;
	for( i=0; i<ICAN_SLAVE_NUM; i++ ){
		if( p->slave_id == id )
			return p;
	}
	if( i==ICAN_SLAVE_NUM ){
		for( i=0; i<ICAN_SLAVE_NUM; i++ ){
			if( p->slave_id==0 ){
				return p;
			}
		}
	}
	LOG_W("the receive handle is full");
	return RT_NULL;
}

static uint8_t ican_parse_msg(ican_port_t *port, ican_fd_frame_t * pmsg, uint8_t * buff, uint8_t len)
{
	INT8U ret = 0;
	ican_sub_recv_t * ptmp,itemp;
	
	rt_memset(&itemp, 0, sizeof(ican_sub_recv_t));
	
	ptmp = &itemp;
	
	switch(pmsg->id.func_id)
	{
		case ICAN_FUC_READ :
			if(ptmp->length <= 7)
			{
				if(len > ptmp->length)
					rt_memcpy(buff, pmsg->frame_data + 1, ptmp->length);
				else
					rt_memcpy(buff, pmsg->frame_data + 1, len);
			}
			else
			{
				if(len > ptmp->length)
					rt_memcpy(buff, ptmp->buff, ptmp->length);
				else
					rt_memcpy(buff, ptmp->buff, len);
			}
			break;
		case ICAN_FUC_WRITE :
		case ICAN_FUC_EST_CONNECT :
		case ICAN_FUC_DEL_CONNECT :
		case ICAN_FUC_DEV_RESET :
			if(pmsg->id.func_id == 0x0f)
				ret = pmsg->frame_data[1];
			else
				ret = ICAN_OK;
			return ret;
		case ICAN_FUC_EVE_TRAGER :break;
	}
	
	return ICAN_OK;
}

/** 
* 主站从从站读数据. 
* 无.
* @param[in]   slave_id:从站地址,src_id:资源ID，offset:资源内偏移地址，len:长度，buff:发送指针. 
* @param[out]  exception_code：返回异常码.  
* @retval  ICAN_OK,ICAN_ERR_TIME_OUT,ICAN_ERR_TRANS. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
uint8_t ican_master_read(uint8_t slave_id, uint8_t src_id, uint8_t offset, uint8_t len, uint8_t * buff)
{
	INT8U state = 0;
	INT8U ret = 0;
	ican_fd_frame_send_t * pmsg,ican_msg;
	ican_sub_recv_t * ptmp,itemp;
	
	rt_memset(&ican_msg, 0, sizeof(ican_fd_frame_send_t));
	rt_memset(&itemp, 0, sizeof(ican_sub_recv_t));
	pmsg = &ican_msg;
	ptmp = &itemp;
	
	if((src_id != ICAN_SRC_DI) && (src_id != ICAN_SRC_AI) && (src_id != ICAN_SRC_SERIAL0) && (src_id != ICAN_SRC_SERIAL1) && 
	   (src_id != ICAN_SRC_USERDEF) && (src_id != ICAN_CFG_DEVICE_INFO) && (src_id != ICAN_CFG_COMMUNICATE) && (src_id != ICAN_CFG_IO_PARAM)){
		
		LOG_D("error source id");
		return ICAN_ERR_SOURCE_ID;
	}
		   
	pmsg->id.src_mac_id = MASTER_MAC_ID;
	pmsg->id.dest_mac_id = slave_id;
	pmsg->id.source_id = src_id;
	pmsg->id.func_id = ICAN_FUC_READ;
	pmsg->id.ack = 0;
	pmsg->offset = offset;
	if( ican_get_src_type(src_id) == ICAN_CFG_SRC )
		pmsg->src_cfg = 1;
	else
		pmsg->src_cfg = 0;
	ptmp->length = 2;
	
	ret = ican_send_msg(pmsg);
	
	if(ret == ICAN_OK){
		
		uint8_t data[8];
		uint16_t msg_len;
		ican_fd_id_t id={0},mask={0};
		
		id.dest_mac_id = slave_id;
		id.src_mac_id = MASTER_MAC_ID;
		id.func_id = ICAN_FUC_READ;
		id.source_id = src_id;
		
				
		mask.dest_mac_id = 0xff;
		mask.src_mac_id = 0xff;
		mask.source_id = 0xff;
		mask.func_id = 0xf;
		if( ican_read_msg(&id, &mask, buff, len, 10, &msg_len) == ICAN_MSG_OK ){
			return ICAN_OK;
		}
		else{
			LOG_D("the slave reply timeout");
			return ICAN_ERR_TIME_OUT;
		}
	}
	else{
		LOG_D("can not send to the slave");
		return ICAN_ERR_TRANS;
	}
}

/** 
* 主站向从站写数据. 
* 无.
* @param[in]   slave_id:从站地址,src_id:资源ID，offset:资源内偏移地址，len:长度，buff:发送指针. 
* @param[out]  exception_code：返回异常码.  
* @retval  ICAN_OK,ICAN_ERR_TIME_OUT,ICAN_ERR_TRANS. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
uint8_t ican_master_write(uint8_t slave_id, uint8_t src_id, uint8_t offset, uint8_t len, uint8_t * buff)
{
	INT8U ret = 0;
	ican_fd_frame_send_t * pmsg,ican_send;
	
	rt_memset(&ican_send, 0, sizeof(ican_fd_frame_send_t));
	
	pmsg = &ican_send;
	
	uint8_t src_addr = ican_get_src_addr(src_id,offset);
	if ( src_addr==ICAN_ERR_ADDR ){
		return ICAN_ERR_SOURCE_ID;
	}
	
	pmsg->id.src_mac_id = 0;//MASTER_MAC_ID;
	pmsg->id.dest_mac_id = slave_id;
	pmsg->id.source_id = 6;//src_addr;
	pmsg->id.func_id = ICAN_FUC_WRITE;
	pmsg->id.ack = 0;
	pmsg->offset = offset;
	if( ican_get_src_type(src_id) == ICAN_CFG_SRC )
		pmsg->src_cfg = 1;
	else
		pmsg->src_cfg = 0;
	pmsg->len = len;
	pmsg->frame_data_ptr = buff;
		
	ret = ican_send_msg(pmsg);
	
	if(ret == RT_EOK){
		
		uint8_t data[8];
		uint16_t msg_len;
		ican_fd_id_t id={0},mask={0};
		
		id.dest_mac_id = MASTER_MAC_ID;
		id.src_mac_id = slave_id;
		id.source_id = ICAN_FUC_WRITE;
		
		mask.dest_mac_id = 0xff;
		mask.src_mac_id = 0xff;
		mask.source_id = 0xff;
		
		if( ican_read_msg(&id, &mask, data, 8, 10, &msg_len) ){
			return ICAN_OK;
		}
		else{
			return ICAN_ERR_TIME_OUT;
		}
	}
	else{
		return ICAN_ERR_TRANS;
	}
}

uint8_t ican_through_read(uint8_t slave_id, uint8_t src_id, uint16_t len, uint8_t * buff)
{
	INT8U ret = 0;
	ican_fd_frame_t * pmsg,ican_msg;
	ican_sub_recv_t * ptmp,itemp;
	
	rt_memset(&ican_msg, 0, sizeof(ican_fd_frame_t));
	rt_memset(&itemp, 0, sizeof(ican_sub_recv_t));
	
	pmsg = &ican_msg;
	ptmp = &itemp;
	
	if(src_id != ICAN_SRC_THROUGH)
		return ICAN_ERR_SOURCE_ID;
}

uint8_t ican_through_write(uint8_t slave_id, uint8_t src_id, uint16_t len, uint8_t * buff)
{
	INT8U ret = 0;
	ican_fd_frame_t * pmsg,ican_msg;
	ican_sub_recv_t * ptmp,itemp;
	
	rt_memset(&ican_msg, 0, sizeof(ican_fd_frame_t));
	rt_memset(&itemp, 0, sizeof(ican_sub_recv_t));
	
	pmsg = &ican_msg;
	ptmp = &itemp;
	
	if(src_id != ICAN_SRC_THROUGH)
		return ICAN_ERR_SOURCE_ID;
}

uint8_t ican_master_event_trager(uint8_t slave_id, uint8_t src_id, uint8_t offset, uint8_t len, uint8_t * buff)
{
	return ICAN_OK;
}

/** 
* 向从设备发送建立连接命令. 
* 无.
* @param[in]   slave_id:从站地址. 
* @param[out]  exception_code：返回异常码.  
* @retval  RT_EOK或RT_ERROR. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
rt_err_t ican_master_establish_connect(uint8_t slave_id, uint8_t *exception_code)
{
	INT8U ret = 0;
	ican_fd_frame_send_t * pmsg,ican_msg;
	
	rt_memset(&ican_msg, 0, sizeof(ican_fd_frame_t));
	
	pmsg = &ican_msg;

	pmsg->id.src_mac_id = MASTER_MAC_ID;
	pmsg->id.dest_mac_id = slave_id;
	pmsg->id.source_id = ICAN_ESTABLISH_CONNECT;
	pmsg->id.func_id = ICAN_FUC_EST_CONNECT;
	pmsg->id.ack = 0;
	static uint8_t frame_data[2];
	frame_data[0] = MASTER_MAC_ID;
	frame_data[1] = CYCLIC_MASTER;
	pmsg->frame_data_ptr = frame_data;
	pmsg->len = 2;
	
	
	ret = ican_send_msg(pmsg);
	
	if(ret == RT_EOK){
		
		uint8_t data[4];
		uint16_t msg_len;
		ican_fd_id_t id={0},mask={0};
		
		id.dest_mac_id = MASTER_MAC_ID;
		id.src_mac_id = slave_id;
		id.source_id = ICAN_ESTABLISH_CONNECT;
		
		mask.dest_mac_id = 0xff;
		mask.src_mac_id = 0xff;
		mask.source_id = 0xff;
		mask.func_id = 0xff;
		
		int ret = ican_read_msg(&id, &mask, data, 4, 10, &msg_len);
		if( ret == ICAN_MSG_OK ){
			slave_src_t *slave_p;
			slave_p = ican_get_slave_handle(slave_id);
			if( slave_p ){
				slave_p->io_param.di_length = data[0];
				slave_p->io_param.di_length = data[1];
				slave_p->io_param.ai_length = data[2];
				slave_p->io_param.ao_length = data[3];
				
				slave_p->slave_id = slave_id;			//保存建立连接的从机地址
				slave_p->slave_status = ICAN_IN_CONNECT;
				
				*exception_code = ICAN_OK;
				return RT_EOK;
			}
			else{
				*exception_code = ICAN_ERR_MEM;
				return RT_ERROR;
			}
		}
		else if( ret == ICAN_MSG_EXCEPTION ){
			//返回异常代码
			*exception_code = data[0];
			return RT_ERROR;
		}
		else if( ret == ICAN_MSG_TIMEOUT ){
			*exception_code = ICAN_ERR_RECV;
			return RT_ERROR;
		}
	}
	else{
		*exception_code = ICAN_ERR_SEND;
		return RT_ERROR;
	}
}

/** 
* 向从设备发送断开连接命令. 
* 无.
* @param[in]   slave_id:从站地址. 
* @param[out]  exception_code：返回异常码.  
* @retval  RT_EOK或RT_ERROR. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
uint8_t ican_master_delete_connect(uint8_t slave_id, uint8_t *exception_code)
{
	INT8U state = 0;
	INT8U ret = 0;
	ican_fd_frame_send_t * pmsg,ican_msg;
	
	rt_memset(&ican_msg, 0, sizeof(ican_fd_frame_t));

	pmsg = &ican_msg;
	
	pmsg->id.src_mac_id = MASTER_MAC_ID;
	pmsg->id.dest_mac_id = slave_id;
	pmsg->id.source_id = ICAN_ESTABLISH_CONNECT;
	pmsg->id.func_id = ICAN_FUC_DEL_CONNECT;
	pmsg->id.ack = 0;
	uint8_t frame_data;
	frame_data = MASTER_MAC_ID;
	pmsg->frame_data_ptr = &frame_data;
	pmsg->len = 1;
		
	ret = ican_send_msg(pmsg);
	
	if(ret == RT_EOK){
		
		uint8_t data[8];
		uint16_t msg_len;
		ican_fd_id_t id={0},mask={0};
		
		id.dest_mac_id = MASTER_MAC_ID;
		id.src_mac_id = slave_id;
		id.source_id = ICAN_FUC_DEL_CONNECT;
		
		mask.dest_mac_id = 0xff;
		mask.src_mac_id = 0xff;
		mask.source_id = 0xff;
		mask.func_id = 0xf;
		
		int ret = ican_read_msg(&id, &mask, data, 8, 10, &msg_len);
		if( ret == ICAN_MSG_OK ){
			slave_src_t *slave_p;
			slave_p = ican_get_slave_handle(slave_id);
			if( slave_p ){
				
				slave_p->slave_status = ICAN_IN_DISCONNECT;
				
				*exception_code = ICAN_OK;
				return RT_EOK;
			}
			else{
				*exception_code = ICAN_ERR_MEM;
				return RT_ERROR;
			}
		}
		else if( ret == ICAN_MSG_EXCEPTION ){
			//返回异常代码
			*exception_code = data[0];
			return RT_ERROR;
		}
		else if( ret == ICAN_MSG_TIMEOUT ){
			*exception_code = ICAN_ERR_RECV;
			return RT_ERROR;
		}
	}
	else{
		*exception_code = ICAN_ERR_SEND;
		return RT_ERROR;
	}
}

/** 
* 向从设备发送复位命令. 
* 无.
* @param[in]   slave_id:从站地址. 
* @param[out]  exception_code：返回异常码.  
* @retval  RT_EOK或RT_ERROR. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
uint8_t ican_master_device_reset(uint8_t slave_id, uint8_t *exception_code)
{
	INT8U ret = 0;
	ican_fd_frame_send_t * pmsg,ican_msg;
	
	rt_memset(&ican_msg, 0, sizeof(ican_fd_frame_t));

	pmsg = &ican_msg;

	pmsg->id.src_mac_id = MASTER_MAC_ID;
	pmsg->id.dest_mac_id = slave_id;
	pmsg->id.source_id = 0xff;
	pmsg->id.func_id = ICAN_FUC_DEV_RESET;
	pmsg->id.ack = 0;
	uint8_t frame_data = slave_id;
	pmsg->frame_data_ptr = &frame_data;
	pmsg->len = 1;
	
	ret = ican_send_msg(pmsg);
	
	if(ret == RT_EOK){
		
		uint8_t data[8];
		uint16_t msg_len;
		ican_fd_id_t id={0},mask={0};
		
		id.dest_mac_id = MASTER_MAC_ID;
		id.src_mac_id = slave_id;
		id.source_id = ICAN_FUC_DEV_RESET;
		
		mask.dest_mac_id = 0xff;
		mask.src_mac_id = 0xff;
		mask.func_id = 0xf;
		
		int ret = ican_read_msg(&id, &mask, data, 8, 10, &msg_len);
		if( ret == ICAN_MSG_OK ){
			return RT_EOK;
		}
		else if( ret == ICAN_MSG_EXCEPTION ){
			//返回异常代码
			*exception_code = data[0];
			return RT_ERROR;
		}
		else if( ret == ICAN_MSG_TIMEOUT ){
			*exception_code = ICAN_ERR_RECV;
			return RT_ERROR;
		}
	}
	else{
		*exception_code = ICAN_ERR_SEND;
		return RT_ERROR;
	}
}

/** 
* 向CAN网内检查是否有相同的地址的ICAN设备. 
* 无.
* @param[in]   slave_id:从站地址. 
* @param[out]  exception_code：返回异常码.  
* @retval  RT_EOK或RT_ERROR. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
uint8_t ican_master_mac_check(uint8_t slave_id, uint8_t *exception_code)
{
	INT8U ret = 0;
	INT32U tmp = 0;
	ican_fd_frame_send_t * pmsg,ican_msg;
	
	
	rt_memset(&ican_msg, 0, sizeof(ican_fd_frame_t));
	
	pmsg = &ican_msg;
	
	pmsg->id.src_mac_id = MASTER_MAC_ID;
	pmsg->id.dest_mac_id = MASTER_MAC_ID;
	pmsg->id.source_id = 0x00;
	pmsg->id.func_id = ICAN_FUC_MAC_CHECK;
	pmsg->id.ack = 0;
	uint8_t frame_data[5];
	tmp = SERIAL_NUMBER;
	frame_data[0] = MASTER_MAC_ID;
	frame_data[1] = tmp & 0xff;
	frame_data[2] = (tmp >> 8) & 0xff;
	frame_data[3] = (tmp >> 16) & 0xff;
	frame_data[4] = (tmp >> 24) & 0xff;
	pmsg->frame_data_ptr = frame_data;
	pmsg->len = 5;

	ret = ican_send_msg(pmsg);
	
	if(ret == RT_EOK){
		
		uint8_t data[8];
		uint16_t msg_len;
		ican_fd_id_t id={0},mask={0};
		
		id.dest_mac_id = MASTER_MAC_ID;
		id.src_mac_id = slave_id;
		id.source_id = ICAN_FUC_MAC_CHECK;
		
		mask.dest_mac_id = 0xff;
		mask.src_mac_id = 0xff;
		mask.func_id = 0xf;
		
		int ret = ican_read_msg(&id, &mask, data, 8, 1000, &msg_len);
		if( ret == ICAN_MSG_OK ){
			
			imaster.mac_conflict = 1;			//地址冲突
			*exception_code = ICAN_ERR_RECV;
			return RT_ERROR;
		}
		else if( ret == ICAN_MSG_TIMEOUT ){
			imaster.mac_conflict = 0;			//地址允许使用
			*exception_code = ICAN_OK;
			return RT_EOK;
		}
	}
	else{
		*exception_code = ICAN_ERR_SEND;
		return RT_ERROR;
	}
}

void ican_master_task(void *parameter)
{
	//判断是否重复MAC地址
	uint8_t exce_code;
	if( ican_master_mac_check(MASTER_MAC_ID, &exce_code) != ICAN_OK )
		return;
	
	//连接从站
	for( uint8_t i=0;i<ICAN_SLAVE_NUM; i++ ){
		ican_master_establish_connect(imaster.slave_src_list[i].slave_id, &exce_code);
	}
	
	while(1){

		for( uint8_t i=0; i<ICAN_SLAVE_NUM; i++ ){
			
			if( imaster.slave_src_list[i].slave_status == ICAN_IN_CONNECT ){
				
				slave_src_t *slave_src_ptr = &imaster.slave_src_list[i];
				
				if( slave_src_ptr->poll_time++ > CYCLIC_MASTER ){
					
					slave_src_ptr->poll_time = 0;
					
					#if ICAN_DI_LEN>0
					if( slave_src_ptr->io_param.di_length != 0 ){
						ican_master_read(slave_src_ptr->slave_id, ICAN_SRC_DI, 0, slave_src_ptr->io_param.di_length, slave_src_ptr->io_src.di_data);
					}
					#endif
					
					#if ICAN_DO_LEN>0
					if( slave_src_ptr->io_param.do_length != 0 ){
						ican_master_write(slave_src_ptr->slave_id, ICAN_SRC_DO, 0, slave_src_ptr->io_param.do_length, slave_src_ptr->io_src.do_data);
					}
					#endif
					
					#if ICAN_AI_LEN>0
					if( slave_src_ptr->io_param.ai_length != 0 ){
						ican_master_read(slave_src_ptr->slave_id, ICAN_SRC_AI, 0, slave_src_ptr->io_param.ai_length,slave_src_ptr->io_src.ai_data);
					}
					#endif
					
					#if ICAN_AO_LEN>0
					if( slave_src_ptr->io_param.ao_length != 0 ){
						ican_master_write(slave_src_ptr->slave_id, ICAN_SRC_AO, 0, slave_src_ptr->io_param.ao_length,slave_src_ptr->io_src.ao_data);
					}
					#endif
					
					#if ICAN_SER0_LEN>0
					if( slave_src_ptr->io_param.serial0_length != 0 ){
						ican_master_write(slave_src_ptr->slave_id, ICAN_SRC_SERIAL0, 0, slave_src_ptr->io_param.serial0_length, slave_src_ptr->io_src.serial0_data);
					}
					#endif
					
					#if ICAN_SER1_LEN>0
					if( slave_src_ptr->io_param.serial1_length != 0 ){
						ican_master_write(slave_src_ptr->slave_id, ICAN_SRC_SERIAL1, 0, slave_src_ptr->io_param.serial1_length, slave_src_ptr->io_src.serial1_data);
					}
					#endif
				}
			}
		}
		rt_thread_delay(10);
	}
}

/** 
* ican master主站初始化及CAN驱动初始化. 
* 无.
* @param[in]   dev_name:CAN设备名称,baudrate:波特率. 
* @param[out]  无.  
* @retval  无 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
int ican_master_init(char *dev_name, uint32_t baudrate)
{
	INT8U i = 0;
	INT8U addr = 0;
	ican_master * p;

	p 													= &imaster;
	pimaster 											= p;

//	p->dev_channel 										= channel;
	p->dev_addr 										= MASTER_MAC_ID;
	p->dev_status 										= ICAN_IDLE_STATUS;
	p->slave_setup_check 								= 0;
//	p->pstat 											= &can_stat[channel];
	p->dev_inf.vendor_id 								= VENDOR_ID;
	p->dev_inf.product_type 							= PRODUCT_TYPE;
	p->dev_inf.product_code 							= PRODUCT_CODE;
	p->dev_inf.hardware_version 						= HARDWARE_VERSION;
	p->dev_inf.firmware_version 						= FIRMWARE_VERSION;
	p->dev_inf.serial_number 							= SERIAL_NUMBER;

	addr 												= ican_get_device_addr();
	p->com_cfg.dev_mac_id 								= addr & 0x3f;
	p->com_cfg.baud_rate 								= (addr >> 6) & 0x03;
	p->com_cfg.user_baud_rate 							= USER_BAUD_RATE;
	p->com_cfg.cyclic_param 							= CYCLIC_PARAM;
	p->com_cfg.cyclic_master 							= CYCLIC_MASTER;
	p->com_cfg.cos_type 								= COS_TYPE;
	p->com_cfg.master_mac_id 							= MASTER_MAC_ID;

	for(i = 0; i < ICAN_SLAVE_NUM; i++)
	{
		p->slave_src_list[i].slave_status 				= ICAN_IN_DISCONNECT;
		p->slave_src_list[i].slave_id					= i+1;
		
		p->slave_src_list[i].io_param.di_length 		= ICAN_DI_LEN;
		p->slave_src_list[i].io_param.do_length 		= ICAN_DO_LEN;
		p->slave_src_list[i].io_param.ai_length 		= ICAN_AI_LEN;
		p->slave_src_list[i].io_param.ao_length 		= ICAN_AO_LEN;
		p->slave_src_list[i].io_param.serial0_length 	= ICAN_SER0_LEN;
		p->slave_src_list[i].io_param.serial1_length 	= ICAN_SER1_LEN;
		p->slave_src_list[i].io_param.userdef_length 	= ICAN_USER_LEN; 
		
		rt_memset((void *)&(p->slave_src_list[i].io_src), 0, sizeof(ican_fd_io_src_t));
		rt_memset((void *)&(p->slave_src_list[i].dev_inf), 0, sizeof(ican_fd_dev_info_t));
		rt_memset((void *)&(p->slave_src_list[i].com_cfg), 0, sizeof(ican_fd_com_info_t));
	}
	
	RT_ASSERT(dev_name != RT_NULL);
	
    struct rt_can_filter_item items[] =
    {
		//接收本地ID
		RT_CAN_FILTER_ITEM_INIT(ICAN_ID_ITEM_INIT(1,0,0,0,0),1,0,0,ICAN_ID_ITEM_INIT(0xff,0,0,0,0), RT_NULL, RT_NULL),
		//接收ICAN广播数据
		RT_CAN_FILTER_ITEM_INIT(ICAN_ID_ITEM_INIT(0xff,0,0,0,0),1,0,0,ICAN_ID_ITEM_INIT(0xff,0,0,0,0), RT_NULL, RT_NULL),
    };
	
    struct rt_can_filter_config cfg = {2, 1, items}; /* 一共有 2 个过滤表 */
	can_init(&p->ican_comm.can_port,"can1",CAN125kBaud, CAN125kBaud, &cfg);
	
	ican_comm_init(&imaster.ican_comm);
	
	rt_thread_t thread=RT_NULL;
	thread = rt_thread_create("ican_master", ican_master_task, NULL, 1024, 15, 10);
	
	if (thread != RT_NULL){
		
		rt_thread_startup(thread);
	}
	else{
		return RT_ERROR;
	}
}
//INIT_COMPONENT_EXPORT(ican_master_init);
