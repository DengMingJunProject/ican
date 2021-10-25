
#include "ican.h"
#include "ican_slave.h"
#include "ican_convert.h"

#define LOG_TAG             "ican.slave"
#include <ulog.h>

static ican_slave_t islave;
static ican_slave_t *pican_slave;

INT16U ican_get_vendor_id(void)
{
	return pican_slave->dev_inf.vendor_id;
}
INT16U ican_get_product_type(void)
{
	return pican_slave->dev_inf.product_type;
}
INT16U ican_get_product_code(void)
{
	return pican_slave->dev_inf.product_code;
}
INT16U ican_get_hardware_version(void)
{
	return pican_slave->dev_inf.hardware_version;
}
INT16U ican_get_firmware_version(void)
{
	return pican_slave->dev_inf.firmware_version;
}
INT32U ican_get_serial_number(void)
{	
	return pican_slave->dev_inf.serial_number;
}
INT8U ican_get_dev_mac_id(void)
{
	return pican_slave->com_cfg.dev_mac_id;
}
void ican_set_dev_mac_id(INT8U id)
{
	pican_slave->com_cfg.dev_mac_id = id;
}
INT8U ican_get_baud_rate(void)
{
	return pican_slave->com_cfg.baud_rate;
}
void ican_set_baud_rate(INT8U rate)
{
	pican_slave->com_cfg.baud_rate = rate;
}
INT32U ican_get_user_baud_rate(void)
{
	return pican_slave->com_cfg.user_baud_rate;
}
void ican_set_user_baud_rate(INT32U usr_rate)
{
	pican_slave->com_cfg.user_baud_rate = usr_rate;
}
INT8U ican_get_cyclic_param(void)
{
	return pican_slave->com_cfg.cyclic_param;
}
void ican_set_cyclic_param(INT8U time)
{
	pican_slave->com_cfg.cyclic_param = time;
}
INT8U ican_get_cyclic_master(void)
{
	return pican_slave->com_cfg.cyclic_master;
}
void ican_set_cyclic_master(INT8U time)
{
	pican_slave->com_cfg.cyclic_master = time;
}
INT8U ican_get_cos_type(void)
{
	return pican_slave->com_cfg.cos_type;
}
void ican_set_cos_type(INT8U type)
{
	pican_slave->com_cfg.cos_type = type;
}
INT8U ican_get_master_mac_id(void)
{
	return pican_slave->com_cfg.master_mac_id;
}
void ican_set_master_mac_id(INT8U id)
{
	pican_slave->com_cfg.master_mac_id = id;
}

static uint8_t ican_read_di(uint8_t offset, uint8_t *length, uint8_t * p)
{
#if (ICAN_DI_LEN > 0)

	INT16U len = 0;
	ican_fd_io_src_t * pio;
	ican_fd_io_param_t * pa;
	
	pio = &pican_slave->io_src;
	pa = &pican_slave->io_param;

	if(pa->di_length == 0)
		return ICAN_ERR_LEN_ZERO;
	
	if(offset > pa->di_length)
		return ICAN_ERR_PARAM;
	else
	{
		if(*length > pa->di_length)
			len = pa->di_length;
		else
			len = *length;
			
		rt_memcpy(p, pio->di_data + offset, len);
		*length = len;
	} 
	
	return ICAN_OK;
#else
	return ICAN_ERR_LEN_ZERO;
#endif
}

static uint8_t ican_read_ai(uint8_t offset, uint8_t *length, uint8_t * p)
{
#if (ICAN_AI_LEN > 0)

	INT16U len = 0;
	ican_fd_io_src_t * pio;
	ican_fd_io_param_t * pa;
	
	pio = &pican_slave->io_src;
	pa = &pican_slave->io_param;

	if(pa->ai_length == 0)
		return ICAN_ERR_LEN_ZERO;
	
	if(offset > pa->ai_length)
		return ICAN_ERR_PARAM;
	else
	{
		if(*length > pa->ai_length)
			len = pa->ai_length;
		else
			len = *length;
		
		rt_memcpy(p, pio->ai_data + offset, len);
		*length = len;
	}	
	
	return ICAN_OK;

#else

	return ICAN_ERR_LEN_ZERO;

#endif
}
static uint8_t ican_read_serial0(uint8_t offset, uint8_t *length, uint8_t * p)
{
#if (ICAN_SER0_LEN > 0)

	INT16U len = 0;
	ican_fd_io_src_t * pio;
	ican_fd_io_param_t * pa;
	
	pio = &pican_slave->io_src;
	pa = &pican_slave->io_param;

	if(pa->serial0_length == 0)
		return ICAN_ERR_LEN_ZERO;
	
	if(offset > pa->serial0_length)
		return ICAN_ERR_PARAM;
	else
	{
		if(*length > pa->serial0_length)
			len = pa->serial0_length;
		else
			len = *length;
		
		rt_memcpy(p, pio->serial0_data + offset, len);
		*length = len;
	}	
	
	return ICAN_OK;

#else

	return ICAN_ERR_LEN_ZERO;

#endif
}						
static uint8_t ican_read_serial1(uint8_t offset, uint8_t *length, uint8_t * p)
{
#if (ICAN_SER1_LEN > 0)

	INT16U len = 0;
	ican_fd_io_src_t * pio;
	ican_fd_io_param_t * pa;
	
	pio = &pican_slave->io_src;
	pa = &pican_slave->io_param;

	if(pa->serial1_length == 0)
		return ICAN_ERR_LEN_ZERO;
	
	if(offset > pa->serial1_length)
		return ICAN_ERR_PARAM;
	else
	{
		if(*length > pa->serial1_length)
			len = pa->serial1_length;
		else
			len = *length;
		
		rt_memcpy(p, pio->serial1_data + offset, len);
		*length = len;
	}	
	
	return ICAN_OK;

#else
	
	return ICAN_ERR_LEN_ZERO;

#endif
}						
static uint8_t ican_read_userdef(uint8_t offset, uint8_t *length, uint8_t * p)	
{
#if (ICAN_USER_LEN > 0)

	INT16U len = 0;
	ican_fd_io_src_t * pio;
	ican_fd_io_param_t * pa;
	
	pio = &pican_slave->io_src;
	pa = &pican_slave->io_param;

	if(pa->userdef_length == 0)
		return ICAN_ERR_LEN_ZERO;
	
	if(offset > pa->userdef_length)
		return ICAN_ERR_PARAM;
	else
	{
		if(*length > pa->userdef_length)
			len = pa->userdef_length;
		else
			len = *length;
		
		rt_memcpy(p, pio->userdef_data + offset, len);
		*length = len;
	}	
	
	return ICAN_OK;

#else

	return ICAN_ERR_LEN_ZERO;

#endif
}						
static uint8_t ican_read_device_info(uint8_t offset, uint8_t *length, uint8_t * p)
{
	INT16U len = 0;
	INT16U tmp = 0;
	ican_fd_dev_info_t * pinf;
	
	tmp = sizeof(ican_fd_dev_info_t);
	pinf = &pican_slave->dev_inf;
	
	if(offset > tmp)
		return ICAN_ERR_PARAM;
	else
	{
		if(*length > tmp)
			len = tmp;
		else
			len = *length;
		
		rt_memcpy(p, pinf + offset, len);
		*length = len;
	}	
	
	return ICAN_OK;
}					
static uint8_t ican_read_communicate(uint8_t offset, uint8_t *length, uint8_t * p)	
{
	INT16U len = 0;
	INT16U tmp = 0;
	ican_fd_com_info_t * pinf;
	
	tmp = sizeof(ican_fd_com_info_t);
	pinf = &pican_slave->com_cfg;
	
	if(offset > tmp)
		return ICAN_ERR_PARAM;
	else
	{
		if(*length > tmp)
			len = tmp;
		else
			len = *length;
		
		rt_memcpy(p, pinf + offset, len);
		*length = len;
	}	
	
	return ICAN_OK;
}			
static uint8_t ican_read_io_param(uint8_t offset, uint8_t *length, uint8_t * p)
{
	INT16U len = 0;
	INT16U tmp = 0;
	ican_fd_io_param_t * pinf;
	
	tmp = sizeof(ican_fd_io_param_t);
	pinf = &pican_slave->io_param;
	
	if(offset > tmp)
		return ICAN_ERR_PARAM;
	else
	{
		if(*length > tmp)
			len = tmp;
		else
			len = *length;
		
		rt_memcpy(p, pinf + offset, len);
		*length = len;
	}	
	
	return ICAN_OK;
}	
static uint8_t ican_write_do(uint8_t offset, uint8_t length, uint8_t * p)
{
#if (ICAN_DO_LEN > 0)

	INT16U len = 0;
	ican_fd_io_src_t * pio;
	ican_fd_io_param_t * pa;
	
	pio = &pican_slave->io_src;
	pa = &pican_slave->io_param;

	if(pa->do_length == 0)
		return ICAN_ERR_LEN_ZERO;
	
	if(offset > pa->do_length)
		return ICAN_ERR_PARAM;
	else
	{
		if(length > pa->do_length)
			len = pa->do_length;
		else
			len = length;
			
		rt_memcpy(pio->do_data + offset, p, len);
	}

	return ICAN_OK;	

#else

	return ICAN_ERR_LEN_ZERO;

#endif
}
static uint8_t ican_write_ao(uint8_t offset, uint8_t length, uint8_t * p)
{
#if (ICAN_AO_LEN > 0)

	INT16U len = 0;
	ican_fd_io_src_t * pio;
	ican_fd_io_param_t * pa;
	
	pio = &pican_slave->io_src;
	pa = &pican_slave->io_param;

	if(pa->ao_length == 0)
		return ICAN_ERR_LEN_ZERO;
	
	if(offset > pa->ao_length)
		return ICAN_ERR_PARAM;
	else
	{
		if(length > pa->ao_length)
			len = pa->ao_length;
		else
			len = length;
			
		rt_memcpy(pio->ao_data + offset, p, len);
	}

	return ICAN_OK;

#else

	return ICAN_ERR_LEN_ZERO;

#endif
}								
static INT8U ican_write_serial0(INT8U offset, INT8U length, INT8U * p)
{
#if (ICAN_SER0_LEN > 0)

	INT16U len = 0;
	ican_fd_io_src_t * pio;
	ican_fd_io_param_t * pa;
	
	pio = &pican_slave->io_src;
	pa = &pican_slave->io_param;

	if(pa->serial0_length == 0)
		return ICAN_ERR_LEN_ZERO;
	
	if(offset > pa->serial0_length)
		return ICAN_ERR_PARAM;
	else
	{
		if(length > pa->serial0_length)
			len = pa->serial0_length;
		else
			len = length;
			
		rt_memcpy(pio->serial0_data + offset, p, len);
	}

	return ICAN_OK;

#else

	return ICAN_ERR_LEN_ZERO;

#endif
}						
static INT8U ican_write_serial1(INT8U offset, INT8U length, INT8U * p)
{
#if (ICAN_SER1_LEN > 0)

	INT16U len = 0;
	ican_fd_io_src_t * pio;
	ican_fd_io_param_t * pa;
	
	pio = &pican_slave->io_src;
	pa = &pican_slave->io_param;

	if(pa->serial1_length == 0)
		return ICAN_ERR_LEN_ZERO;
	
	if(offset > pa->serial1_length)
		return ICAN_ERR_PARAM;
	else
	{
		if(length > pa->serial1_length)
			len = pa->serial1_length;
		else
			len = length;
			
		rt_memcpy(pio->serial1_data + offset, p, len);
	}

	return ICAN_OK;

#else

	return ICAN_ERR_LEN_ZERO;

#endif
}						
static INT8U ican_write_userdef(INT8U offset, INT8U length, INT8U * p)
{
#if (ICAN_USER_LEN > 0)

	INT16U len = 0;
	ican_fd_io_src_t * pio;
	ican_fd_io_param_t * pa;
	
	pio = &pican_slave->io_src;
	pa = &pican_slave->io_param;

	if(pa->userdef_length == 0)
		return ICAN_ERR_LEN_ZERO;
	
	if(offset > pa->userdef_length)
		return ICAN_ERR_PARAM;
	else
	{
		if(length > pa->userdef_length)
			len = pa->userdef_length;
		else
			len = length;
			
		rt_memcpy(pio->userdef_data + offset, p, len);
	}

	return ICAN_OK;

#else

	return ICAN_ERR_LEN_ZERO;

#endif
}						
static uint8_t ican_write_io(uint8_t offset, uint8_t length, uint8_t * p)
{
	INT16U len = 0;
	INT16U tmp = 0;
	ican_fd_io_param_t * pinf;
	
	tmp = sizeof(ican_fd_io_param_t);
	pinf = &pican_slave->io_param;
	
	if(offset > tmp)
		return ICAN_ERR_PARAM;
	else
	{
		if(length > tmp)
			len = tmp;
		else
			len = length;
		
		rt_memcpy(pinf + offset, p, len);
	}	
	
	return ICAN_OK;
}

static uint8_t ican_slave_reply(ican_fd_frame_t *pmsg, uint8_t err)
{
	ican_fd_frame_send_t send_msg;
	send_msg.id.dest_mac_id = pmsg->id.src_mac_id;
	send_msg.id.src_mac_id = pmsg->id.dest_mac_id;
	send_msg.id.source_id = pmsg->id.source_id;
	send_msg.id.ack = 1;
	send_msg.frame_data_ptr = &err;
	
	if( err != ICAN_OK ){
		send_msg.len = 1;
		send_msg.id.func_id = ICAN_FUC_ERROR;
	}
	else{
		send_msg.len = 0;
		send_msg.id.func_id = pmsg->id.func_id;
	}
	ican_send_msg(&send_msg);
}

static uint8_t ican_slave_read(ican_fd_frame_t * pmsg)
{
	uint8_t src_id = 0;
	uint8_t offset = 0;
	uint8_t length = 0;
	uint8_t ret = 0;
	src_id = ican_get_source_id(pmsg);
	offset = ican_get_offset(pmsg);
	length = ican_get_length(pmsg);
	
	ican_fd_frame_send_t send_msg;
	send_msg.id.dest_mac_id = pmsg->id.src_mac_id;
	send_msg.id.src_mac_id = pmsg->id.dest_mac_id;
	send_msg.id.source_id = pmsg->id.source_id;
	send_msg.id.func_id = pmsg->id.func_id;
	send_msg.id.ack = 1;
	uint8_t buff[CAN_DATA_SIZE];
	send_msg.frame_data_ptr = buff;
	
	switch(src_id)
	{
		case ICAN_SRC_DI 			:ret = ican_read_di(offset, &length, buff);break;
		case ICAN_SRC_AI 			:ret = ican_read_ai(offset, &length, buff);break;
		case ICAN_SRC_SERIAL0 		:ret = ican_read_serial0(offset, &length, buff);break;							
		case ICAN_SRC_SERIAL1 		:ret = ican_read_serial1(offset, &length, buff);break;							
		case ICAN_SRC_USERDEF 		:ret = ican_read_userdef(offset, &length, buff);break;							
		case ICAN_CFG_DEVICE_INFO 	:ret = ican_read_device_info(offset, &length, buff);break;					
		case ICAN_CFG_COMMUNICATE 	:ret = ican_read_communicate(offset, &length, buff);break;					
		case ICAN_CFG_IO_PARAM 		:ret = ican_read_io_param(offset, &length, buff);break;	
		default 					:ret = ICAN_ERR_SOURCE_ID;							
	}
	
	if( ret == ICAN_OK ){
		send_msg.len = length;
		ican_send_msg(&send_msg);
	}
	else{
		send_msg.len = 1;
		send_msg.id.func_id = ICAN_FUC_ERROR;
		buff[0] = ret;
		ican_send_msg(&send_msg);
	}

	return ret;	
}
static uint8_t ican_slave_write(ican_fd_frame_t * pmsg)
{
	uint8_t src_id = 0;
	uint8_t offset = 0;
	uint8_t length = 0;
	uint8_t *buff = NULL;
	uint8_t ret = 0;

	src_id = ican_get_source_id(pmsg);
	offset = ican_get_offset(pmsg);
	length = ican_get_length(pmsg);

	buff = pmsg->frame_data	+ 3;
		
	switch(src_id)
	{
		case ICAN_SRC_DO 		:ret = ican_write_do(offset, length, buff);break;
		case ICAN_SRC_AO 		:ret = ican_write_ao(offset, length, buff);break;								
		case ICAN_SRC_SERIAL0 	:ret = ican_write_serial0(offset, length, buff);break;						
		case ICAN_SRC_SERIAL1 	:ret = ican_write_serial1(offset, length, buff);break;						
		case ICAN_SRC_USERDEF 	:ret = ican_write_userdef(offset, length, buff);break;						
		case ICAN_CFG_IO_CFG	:ret = ican_write_io(offset, length, buff);break;						
		default 				:ret = ICAN_ERR_SOURCE_ID;
	}
	
	ican_slave_reply(pmsg,ret);
	return ret;
}
static uint8_t ican_slave_event_triger(void)
{
	return ICAN_OK;
}

static uint8_t ican_slave_timer_triger(void)
{
	return ICAN_OK;
}

static uint8_t ican_slave_establish_connect(ican_fd_frame_t * pmsg)
{
	uint8_t last_master_id = 0;
	uint8_t src_id = 0;
	uint8_t ret;

	src_id = ican_get_source_id(pmsg);
	last_master_id = ican_get_master_mac_id();
	
	if(src_id == 0xf7)
	{
		if((last_master_id != 0xff) && (last_master_id != pmsg->frame_data[2])){
			ret = ICAN_ERR_NODE_BUSY;
			ican_slave_reply(pmsg,ret);
		}
		else
		{
			ican_set_master_mac_id(pmsg->frame_data[2]);
			ican_set_cyclic_master(pmsg->frame_data[3]);
			islave.slave_status = ICAN_IN_CONNECT;
			ret = ICAN_OK;
		}
	}
	else{
		ret = ICAN_ERR_SOURCE_ID;
		ican_slave_reply(pmsg,ret);
	}
	
	return ret;
}
static uint8_t ican_slave_delete_connect(ican_fd_frame_t * pmsg)
{
	uint8_t src_id = 0;
	uint8_t last_master_id = 0;
	uint8_t ret;

	src_id = ican_get_source_id(pmsg);
	last_master_id = ican_get_master_mac_id();
	
	if(src_id == 0xf7)
	{
		if(last_master_id != 0xff)
		{
			if(last_master_id != pmsg->frame_data[2])
				ret = ICAN_ERR_DEL_NODE;
			else
			{
				ican_set_master_mac_id(0xff);
				ican_set_cyclic_master(0x00);
				
				pican_slave->slave_status = ICAN_IN_DISCONNECT;
				
				ican_fd_frame_send_t send_msg;
				send_msg.id.dest_mac_id = pmsg->id.src_mac_id;
				send_msg.id.src_mac_id = pmsg->id.dest_mac_id;
				send_msg.id.source_id = pmsg->id.source_id;
				send_msg.id.ack = 1;
				uint8_t reply[4];
				send_msg.frame_data_ptr = reply;
				send_msg.len= 4;
				reply[0] = ICAN_DI_LEN;
				reply[1] = ICAN_DO_LEN;
				reply[2] = ICAN_AI_LEN;
				reply[3] = ICAN_AO_LEN;
				
				ican_send_msg(&send_msg);
				
				ret = ICAN_OK;
			}
		}
		else
			ret = ICAN_ERR_NODE_EXIST;
	}
	else
		ret = ICAN_ERR_SOURCE_ID;
	
	ican_slave_reply(pmsg,ret);
	return ret;	
}
static uint8_t ican_slave_device_reset(ican_fd_frame_t * pmsg)
{
	uint8_t ret;
	if( islave.slave_status==ICAN_IN_CONNECT ){
		ret = ICAN_ERR_COMMUNICATE;
	}
	if( pmsg->frame_data[2] == ican_get_device_addr() ){
		ret = ICAN_OK;
	}
	else{
		ret = ICAN_ERR_PARAM;
	}
	ican_slave_reply(pmsg,ret);
	return ret;
}
static uint8_t ican_slave_mac_check(ican_fd_frame_t * pmsg)
{
	if( pmsg->frame_data[2] == ican_get_device_addr() ){
		
		ican_fd_frame_send_t send_msg;
		send_msg.id.dest_mac_id = pmsg->id.src_mac_id;
		send_msg.id.src_mac_id = pmsg->id.dest_mac_id;
		send_msg.id.source_id = pmsg->id.source_id;
		send_msg.id.func_id = pmsg->id.func_id;
		send_msg.id.ack = 1;
		uint8_t buff[CAN_DATA_SIZE];
		send_msg.frame_data_ptr = buff;
		
		buff[0] = ican_get_device_addr();
		uint32_t id=  ican_get_serial_number();
		rt_memcpy(&buff[1],&id,4);
		send_msg.len = 5;
		ican_send_msg(&send_msg);
		islave.mac_conflict = 1;
	}
}

uint8_t ican_slave_search_id(uint8_t slave_id)
{
	INT8U ret = 0;
	INT32U tmp = 0;
	ican_fd_frame_send_t * pmsg,ican_msg;
	
	
	rt_memset(&ican_msg, 0, sizeof(ican_fd_frame_t));
	
	pmsg = &ican_msg;
	
	pmsg->id.src_mac_id = slave_id;
	pmsg->id.dest_mac_id = slave_id;
	pmsg->id.source_id = 0x00;
	pmsg->id.func_id = ICAN_FUC_MAC_CHECK;
	pmsg->id.ack = 0;
	uint8_t frame_data[5];
	tmp = ican_get_serial_number();
	frame_data[0] = slave_id;
	frame_data[1] = tmp & 0xff;
	frame_data[2] = (tmp >> 8) & 0xff;
	frame_data[3] = (tmp >> 16) & 0xff;
	frame_data[4] = (tmp >> 24) & 0xff;
	pmsg->frame_data_ptr = frame_data;
	pmsg->len = 5;

	islave.mac_conflict = 0;
	ret = ican_send_msg(pmsg);
	rt_thread_delay(1000);
	//1秒内有没收到重复地址的应答
	return islave.mac_conflict;
}

void ican_slave_task(void* parameter)
{
	ican_fd_msg_t msg;
	while(1)
	{
		if( rt_mq_recv(&islave.ican_comm.ican_mq, (void*)&msg, sizeof(ican_fd_msg_t),0) == RT_EOK ){
			
			ican_fd_frame_t	recv_msg;
			recv_msg.id = msg.id;
			recv_msg.dlc = msg.data_len>CAN_DATA_SIZE?CAN_DATA_SIZE:msg.data_len;
			
			rt_memcpy(recv_msg.frame_data,msg.data_ptr,recv_msg.dlc);
			free(msg.data_ptr);
			
			islave.comm_timeout = 0;
			
			switch( msg.id.func_id ){
				
				case ICAN_FUC_WRITE:
					LOG_D("ican func write");
					ican_slave_write(&recv_msg);
					break;
				case ICAN_FUC_READ:
					LOG_D("ican func read");
					ican_slave_read(&recv_msg);
					break;
				case ICAN_FUC_EVE_TRAGER:
					break;
				case ICAN_FUC_EST_CONNECT
					LOG_D("ican func connect");:
					ican_slave_establish_connect(&recv_msg);
					break;
				case ICAN_FUC_DEL_CONNECT:
					LOG_D("ican func delete connect");
					ican_slave_delete_connect(&recv_msg);
					break;
				case ICAN_FUC_DEV_RESET:
					LOG_D("ican func device reset");
					ican_slave_device_reset(&recv_msg);
					break;
				case ICAN_FUC_MAC_CHECK:
					LOG_D("ican func mac check");
					ican_slave_mac_check(&recv_msg);
					break;
				default:
					//获取的消息不需要，重入队列
					if( msg.rein_cnt++ < REIN_THREAD_CNT ){
						rt_mq_urgent(&islave.ican_comm.ican_mq, (void*)&msg, sizeof(ican_fd_msg_t));
						rt_thread_delay(1);
					}
					//没有接收线程，删除
					else{
						LOG_D("delete the no receiver ican pack\r\n");
						rt_free(msg.data_ptr);
					}
					break;
			}
		}
		
		//超过4倍的时间没有收到消息，关闭从站连接
		if( islave.slave_status == ICAN_IN_CONNECT ){
			if( islave.comm_timeout++ >=  ican_get_cyclic_master()*40 ){
				islave.slave_status = ICAN_IN_DISCONNECT;
				
				ican_set_master_mac_id(0xff);
				ican_set_cyclic_master(0x00);
				LOG_D("master disconnect");
			}
			
			if( (islave.cyclic_time++)%ican_get_cyclic_param()*10 == 0 ){
				islave.cyclic_time = 0;
				
				ican_slave_timer_triger();
			}
			
			if( ican_get_cos_type() == COSTYPE_ENABLE ){
				
				ican_slave_event_triger();
			}
		}
		
		rt_thread_delay(1);
	}
}

int ican_slave_init(void)
{
	uint8_t addr = 0;
	ican_slave_t * p;

	p = &islave;
	pican_slave = &islave;

	p->slave_status 			= ICAN_IN_DISCONNECT;

	p->dev_inf.vendor_id 		= VENDOR_ID;
	p->dev_inf.product_type 	= PRODUCT_TYPE;
	p->dev_inf.product_code 	= PRODUCT_CODE;
	p->dev_inf.hardware_version = HARDWARE_VERSION;
	p->dev_inf.firmware_version = FIRMWARE_VERSION;
	p->dev_inf.serial_number 	= SERIAL_NUMBER;
	
	addr 						= ican_get_device_addr(); 
	p->com_cfg.dev_mac_id 		= addr & 0x3f;  
	p->com_cfg.baud_rate 		= (addr >> 6) & 0x03;
	p->com_cfg.user_baud_rate 	= USER_BAUD_RATE;			
	p->com_cfg.cyclic_param 	= CYCLIC_PARAM;
	p->com_cfg.cyclic_master 	= CYCLIC_MASTER;
	p->com_cfg.cos_type 		= COS_TYPE;
	p->com_cfg.master_mac_id 	= MASTER_MAC_ID;

	p->io_param.di_length 		= ICAN_DI_LEN;
	p->io_param.do_length 		= ICAN_DO_LEN;
	p->io_param.ai_length 		= ICAN_AI_LEN;
	p->io_param.ao_length 		= ICAN_AO_LEN;
	p->io_param.serial0_length 	= ICAN_SER0_LEN;
	p->io_param.serial1_length 	= ICAN_SER1_LEN;
	p->io_param.userdef_length 	= ICAN_USER_LEN;

	rt_memset(&pican_slave->io_src, 0, sizeof(ican_fd_io_src_t));
	
	struct rt_can_filter_item items[] =
    {
		//如需应答，必成对设置，否则会被过滤
		RT_CAN_FILTER_EXT_INIT(ICAN_ID_ITEM_INIT(1,0,0,8,0), RT_NULL, RT_NULL),
		RT_CAN_FILTER_EXT_INIT(ICAN_ID_ITEM_INIT(0,1,1,8,0), RT_NULL, RT_NULL),
		//接收ICAN广播数据
		RT_CAN_FILTER_ITEM_INIT(ICAN_ID_ITEM_INIT(0xff,0,0,0,0),1,0,0,ICAN_ID_ITEM_INIT(0xff,0,0,0,0), RT_NULL, RT_NULL),
    };
	
    struct rt_can_filter_config cfg = {3, 1, items}; /* 一共有 5 个过滤表 */
	can_init(&p->ican_comm.can_port,"can1",CAN125kBaud, CAN125kBaud, &cfg);
	
	ican_comm_init(&islave.ican_comm);
	
	rt_thread_t thread=RT_NULL;
	thread = rt_thread_create("ican_slave", ican_slave_task, NULL, 1024, 15, 10);
	
	if (thread != RT_NULL){
		
		rt_thread_startup(thread);
	}
	else{
		LOG_E("ican slave stack start failed");
		return RT_ERROR;
	}
}
