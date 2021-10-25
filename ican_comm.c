/** 
* @file         ican_comm.c 
* @brief        ICAN的组包分包应答处理. 
* @details  	ICAN主站及从站共用通讯代码. 
* @author       ken deng 
* @date     	2020-05-26 
* @version  	A001 
* @par Copyright (c):  
*
* @par History:          
*   version: ken deng, 2020-05-26, 建立\n 
*/ 
#include "ican_comm.h"
#include "ican_convert.h"
//#include "dbg.h"

#define LOG_TAG             "ican.comm"
#include <ulog.h>

static struct rt_messagequeue *p_ican_app_mq;
	
/** 
* 获取指定ID的CAN接收数据的缓冲指针. 
* 如没有则新建，如没有空余返回RT_NULL.
* @param[in]   pmsg:ican数据指针. 
* @param[out]  地址或RT_NULL.  
* @retval  无. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
static void* ican_sub_recv_get(void* parameter,ican_fd_frame_t * pmsg)
{
	ican_comm_t *p = (ican_comm_t *)parameter;
	ican_fd_id_t id;
	const uint32_t id_z=0;
	
	for( uint8_t i=0; i<ICAN_MSG_MAX; i++ ){
		
		if( rt_memcmp(&p->sub_recv[i].id,&pmsg->id,sizeof(ican_fd_id_t))==0 ){
			p->sub_recv[i].timeout = 0;
			return &p->sub_recv[i];
		}
		//0号功能码不能使用，用于判断是不是为空
		else if( rt_memcmp(&p->sub_recv[i].id,&id_z,sizeof(ican_fd_id_t))==0 ){
			rt_memset(&p->sub_recv[i],0,sizeof(ican_sub_recv_t));
			p->sub_recv[i].buff = rt_malloc(1);			//先分配一个字节给接收缓冲
			if( p->sub_recv[i].buff ){
				p->sub_recv[i].id=pmsg->id;
				return &p->sub_recv[i];
			}
			else{
				return RT_NULL;
			}
		}
	}
	return RT_NULL;
}

/** 
* 清空指定ID的ICAN缓冲并释放内存. 
* 无.
* @param[in]   id:需删除的ID指针.
* @param[out]  无.  
* @retval  无. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
static void ican_sub_recv_clr(void* parameter,ican_fd_id_t *id)
{
	ican_comm_t *p = (ican_comm_t *)parameter;
	for( uint8_t i=0; i<ICAN_MSG_MAX; i++ ){
		if( rt_memcmp(&p->sub_recv[i].id, id, sizeof(ican_fd_id_t)==0 ) ){
			rt_free(p->sub_recv[i].buff);
			rt_memset(&(p->sub_recv[i]),0,sizeof(ican_sub_recv_t));
		}
	}
}

/** 
* 接收CAN消息. 
* 组包成ICAN消息.
* @param[in]   无. 
* @param[out]  无.  
* @retval  RT_EOK或RT_ERROR. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
static INT8U ican_recv_msg(void *parameter)
{
	ican_comm_t *p = (ican_comm_t *)parameter;
	
	ican_fd_frame_t pmsg;
	static ican_sub_recv_t * ptmp;
		
	rt_memset(&pmsg, 0, sizeof(ican_fd_frame_t));

	if(ican_recv(&pmsg)==RT_EOK)
	{

//		mac_id = MASTER_MAC_ID;
//		if((pmsg.id.dest_mac_id != mac_id) && (pmsg.id.dest_mac_id != 0xff))
//			return ICAN_ERR_MAC_UNMATCH;
//		else
		{
			ptmp = ican_sub_recv_get(parameter,&pmsg);
			if( ptmp==RT_NULL )
				return ICAN_ERR_TRANS;
			
			ptmp->cur_flag = ican_get_split_flag(&pmsg);
			
			if(ptmp->cur_flag == ICAN_NO_SPLIT_SEG)
			{
				ptmp->length = ican_get_length(&pmsg);
				
				if( ptmp->length ){
					ptmp->buff = rt_realloc(ptmp->buff, ptmp->length);
					if( ptmp->buff==RT_NULL ){
						return ICAN_ERR_TRANS; 
					}
					rt_memcpy(ptmp->buff,pmsg.frame_data+2,ptmp->length);
				}
				
				ptmp->recv_comp = 1;
			}
			else
			{
				ptmp->cur_num = ican_get_split_num(&pmsg);
				ptmp->cur_src_id = ican_get_source_id(&pmsg);
							
				if( (ptmp->offset+ican_get_length(&pmsg) <= ICAN_MAX_PACK) && (ptmp->offset+ican_get_length(&pmsg)!=0) ){
					
					if(ptmp->cur_flag == ICAN_SPLIT_SEG_FIRST)
					{
						if((ptmp->cur_num == 0) && (ptmp->old_num == 0))  //??
						{	
							ptmp->old_flag = ptmp->cur_flag;
							ptmp->old_num = ptmp->cur_num;
							
							//改变接收缓冲的大小
							ptmp->buff = rt_realloc(ptmp->buff, ptmp->offset+ican_get_length(&pmsg));
							if( ptmp->buff==RT_NULL ){
								return ICAN_ERR_TRANS; 
							}
							rt_memcpy(ptmp->buff + ptmp->offset , pmsg.frame_data + ICAN_HEADER_LEN, ican_get_length(&pmsg));
							ptmp->offset += ican_get_length(&pmsg);
							ptmp->old_src_id = ican_get_source_id(&pmsg);
	//						rt_memset(&pmsg, 0, sizeof(ican_fd_frame_t));
							
						}
						else
							return ICAN_ERR_TRANS;
					}
					else if(ptmp->cur_flag == ICAN_SPLIT_SEG_MID)
					{
						if(((ptmp->cur_num - 1) == ptmp->old_num) && (ptmp->cur_src_id == ptmp->old_src_id))
						{
							ptmp->old_flag = ptmp->cur_flag;
							ptmp->old_num += 1;
						
							//改变接收缓冲的大小
							ptmp->buff = rt_realloc(ptmp->buff, ptmp->offset+ican_get_length(&pmsg));
							if( ptmp->buff==RT_NULL ){
								return ICAN_ERR_TRANS; 
							}
							rt_memcpy(ptmp->buff + ptmp->offset, pmsg.frame_data + ICAN_HEADER_LEN, ican_get_length(&pmsg));
							ptmp->offset += ican_get_length(&pmsg);;

						}
						else
							return ICAN_ERR_TRANS;
					}
					else
					{
						if(((ptmp->cur_num - 1) == ptmp->old_num) && (ptmp->cur_src_id == ptmp->old_src_id))
						{
							ptmp->old_flag = 0;
							ptmp->old_num = 0;
						
							//改变接收缓冲的大小
							ptmp->buff = rt_realloc(ptmp->buff, ptmp->offset+ican_get_length(&pmsg));
							if( ptmp->buff==RT_NULL ){
								return ICAN_ERR_TRANS; 
							}
							rt_memcpy(ptmp->buff + ptmp->offset, pmsg.frame_data + ICAN_HEADER_LEN, ican_get_length(&pmsg));
							ptmp->offset += ican_get_length(&pmsg);;
							ptmp->length = ptmp->offset;
							ptmp->offset = 0;
							
							ptmp->recv_comp = 1;
							LOG_I("receive complete ican pack %d",ptmp->length);

						}
						else
							return ICAN_ERR_TRANS;
					}
				}
				else{
					return ICAN_ERR_TRANS;
				}
			}
		}
	}
	
	return ICAN_OK;
}

/** 
* 向目标站发送消息. 
* 如起出62bytes，进行分包，最大一数据包3968.
* @param[in]   ican_pack:将要发送的ican数据包. 
* @param[out]  无.  
* @retval  RT_EOK或RT_ERROR. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
INT8U ican_send_msg(ican_fd_frame_send_t * ican_pack)
{
	INT8U cur_trans = 0;
	INT8U cur_trans_state = 0;
	ican_sub_recv_t *ptmp,itemp;
	ican_fd_frame_t *pmsg,ican_msg;
	uint8_t data_len;
	
	rt_memset(&itemp, 0, sizeof(ican_sub_recv_t));
	rt_memset(&ican_msg, 0, sizeof(ican_fd_frame_t));

	pmsg = &ican_msg;
	ptmp = &itemp;
	ptmp->offset = 0;
	ptmp->length = ican_pack->len>ICAN_MAX_PACK?ICAN_MAX_PACK:ican_pack->len;
	ptmp->buff = ican_pack->frame_data_ptr;
	
	rt_memcpy(&pmsg->id, &ican_pack->id, sizeof(ican_fd_id_t));

	if( ((ptmp->length <= MAX_DATA_BUFF-ICAN_HEADER_LEN) && (ican_pack->src_cfg==0)) || ((ptmp->length <= MAX_DATA_BUFF-ICAN_CFG_HEADER_LEN) && (ican_pack->src_cfg==1)) )
	{
		ptmp->old_flag = 0;
		ptmp->old_num = 0;

//		rt_memset(ican_msg.frame_data, 0, MAX_DATA_BUFF);
		
		ican_set_split_flag(pmsg, ptmp->old_flag);
		ican_set_split_num(pmsg, ptmp->old_num);
		if( ican_pack->src_cfg ){
			ican_set_offset(pmsg,ican_pack->offset);
			rt_memcpy(pmsg->frame_data+ICAN_CFG_HEADER_LEN,ptmp->buff,ican_pack->len);
			pmsg->dlc = ican_pack->len+ICAN_CFG_HEADER_LEN;
			ican_set_length(pmsg,ican_pack->len);
		}
		else{
			rt_memcpy(pmsg->frame_data+ICAN_HEADER_LEN,ptmp->buff,ican_pack->len);
			pmsg->dlc = ican_pack->len+ICAN_HEADER_LEN;
			ican_set_length(pmsg,ican_pack->len);
		}
		if( ican_send(pmsg) != RT_EOK ){
			return RT_ERROR;
		}
	}
	else
	{
		cur_trans = 1;
		cur_trans_state = ICAN_SPLIT_SEG_FIRST;
		
		do
		{
//			rt_memset(ican_msg.frame_data,0,MAX_DATA_BUFF);
			
			if(cur_trans_state == ICAN_SPLIT_SEG_FIRST)
			{
				ptmp->old_flag = ICAN_SPLIT_SEG_FIRST;
				ptmp->old_num = 0;
				
				pmsg->dlc = MAX_DATA_BUFF;
				
				if( ican_pack->src_cfg ){
					ican_set_offset(pmsg,ican_pack->offset);
					rt_memcpy(pmsg->frame_data+ICAN_CFG_HEADER_LEN,ptmp->buff + ptmp->offset,pmsg->dlc-ICAN_CFG_HEADER_LEN);
					ptmp->offset += (pmsg->dlc - ICAN_CFG_HEADER_LEN);
					data_len = (pmsg->dlc - ICAN_CFG_HEADER_LEN);
				}
				else{
					rt_memcpy(pmsg->frame_data+ICAN_HEADER_LEN,ptmp->buff + ptmp->offset,pmsg->dlc-ICAN_HEADER_LEN);
					ptmp->offset += (pmsg->dlc - ICAN_HEADER_LEN);
					data_len = (pmsg->dlc - ICAN_HEADER_LEN);
				}

				if((ptmp->length - ptmp->offset) <= MAX_DATA_BUFF-ICAN_HEADER_LEN)
					cur_trans_state = ICAN_SPLIT_SEG_LAST;
				else
					cur_trans_state = ICAN_SPLIT_SEG_MID;
				
			}
			else if(cur_trans_state == ICAN_SPLIT_SEG_MID)
			{
				ptmp->old_flag = ICAN_SPLIT_SEG_MID;
				ptmp->old_num += 1;
				pmsg->dlc = MAX_DATA_BUFF;
				rt_memcpy(pmsg->frame_data + ICAN_HEADER_LEN, ptmp->buff + ptmp->offset, pmsg->dlc - ICAN_HEADER_LEN);
				ptmp->offset += (pmsg->dlc - ICAN_HEADER_LEN);
				data_len = (pmsg->dlc - ICAN_HEADER_LEN);
				
				if((ptmp->length - ptmp->offset) <= MAX_DATA_BUFF-ICAN_HEADER_LEN)
					cur_trans_state = ICAN_SPLIT_SEG_LAST;
			} 
			else
			{
				ptmp->old_flag = ICAN_SPLIT_SEG_LAST;
				ptmp->old_num += 1;
				pmsg->dlc = ptmp->length - ptmp->offset + ICAN_HEADER_LEN;
				rt_memcpy(pmsg->frame_data + ICAN_HEADER_LEN, ptmp->buff + ptmp->offset, pmsg->dlc - ICAN_HEADER_LEN);
				ptmp->offset += (pmsg->dlc - ICAN_HEADER_LEN);
				data_len = (pmsg->dlc - ICAN_HEADER_LEN);
				cur_trans = 0;
			}
			
//			icanm_dump(NULL,10,pmsg->frame_data,pmsg->dlc);
			
			LOG_I("send segment %d",ptmp->old_num);
			ican_set_split_flag(pmsg, ptmp->old_flag);
			ican_set_split_num(pmsg, ptmp->old_num);
			ican_set_length(pmsg,data_len);
			
			if( ican_send(pmsg) != RT_EOK ){
				return RT_ERROR;
			}

//			rt_memset(&ican_msg, 0, sizeof(ican_fd_frame_t));

		}
		while(cur_trans);

		ptmp->offset = 0;
	}
	
	return RT_EOK;
}

/** 
* 接收的ICAN数据进入应用层队列. 
* 无.
* @param[in]   ican数据包指针.
* @param[out]  无.  
* @retval  rt_err_t. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
static rt_err_t ican_mq_in(void *parameter, ican_fd_msg_t *msg)
{
	ican_comm_t *p = (ican_comm_t *)parameter;
	msg->rein_cnt = 0;
	return rt_mq_send(&p->ican_mq, (void*)msg, sizeof(ican_fd_msg_t));
}

/** 
* 过滤读取ICAN消息. 
* 如消息不是本调用函数需要，重入队等待其它调用函数获取，
* 如调用消息超出指定次数都没有获取者则删除.
* @param[in]   id:过滤的ID，mask:掩码，data:接收指针，size:请求接收长度，timeout:超时时间，msg_size:返回实际接收长度. 
* @param[out]  ICAN_MSG_OK,ICAN_MSG_TIMEOUT,ICAN_MSG_EXCEPTION.  
* @retval  无. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
int ican_read_msg(ican_fd_id_t *id, ican_fd_id_t *mask, uint8_t *data, uint16_t size, rt_int32_t timeout, uint16_t *msg_size)
{
	ican_fd_msg_t msg;
	volatile uint32_t *id_ptr,*mask_ptr,*msg_id_ptr,*exce_ptr;
	ican_fd_id_t exce_id;

	id_ptr = (uint32_t*)id;
	mask_ptr = (uint32_t*)mask;
	msg_id_ptr = (uint32_t*)&msg.id;
	exce_ptr = (uint32_t*)&exce_id;
	
	(*exce_ptr) = (*id_ptr);
	exce_id.func_id = ICAN_SRC_EXCEPTION;
	
	volatile rt_tick_t ticks;
	ticks = rt_tick_get();
	
	do{
		if( rt_mq_recv(p_ican_app_mq, (void*)&msg, sizeof(ican_fd_msg_t),1) == RT_EOK ){
			
			//利用掩码过滤不需要的消息
			if( ((*id_ptr)&(*mask_ptr)) == ((*msg_id_ptr)&(*mask_ptr)) 			//比较正常功能码的ID
				|| ((*exce_ptr)&(*mask_ptr)) == ((*msg_id_ptr)&(*mask_ptr)) ){	//比较异常功能码的ID
				
				size = size>msg.data_len?msg.data_len:size;
									
				rt_memcpy(data, msg.data_ptr, size);
				rt_free(msg.data_ptr);
					
				*msg_size = size;
				if( msg.id.func_id == (*id).func_id )
					return ICAN_MSG_OK;
				else{
					return ICAN_MSG_EXCEPTION;
				}
			}
			else{
				//获取的消息不需要，重入队列
				if( msg.rein_cnt++ < REIN_THREAD_CNT ){
					rt_mq_urgent(p_ican_app_mq, (void*)&msg, sizeof(ican_fd_msg_t));
					rt_thread_delay(1);
				}
				//没有接收线程，删除
				else{
					LOG_I("delete the no receiver ican pack");
					rt_free(msg.data_ptr);
					return ICAN_MSG_TIMEOUT;
				}
			}
		}
	}while( (rt_tick_get()-ticks)<timeout );
		
	return ICAN_MSG_TIMEOUT;
}

/** 
* 向发送方返回ACK应答. 
* 无.
* @param[in]   dest_id:目标地址，src_id：源地址，len:接收长度,func:应答的功能码. 
* @param[out]  无.  
* @retval  发送状态 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
static int ican_replay_ack(uint8_t dest_id, uint8_t src_id, uint8_t func)
{
	uint8_t ret = 0;
	ican_fd_frame_send_t * pmsg,ican_send;
	
	rt_memset(&ican_send, 0, sizeof(ican_fd_frame_send_t));
	
	pmsg = &ican_send;
	
	pmsg->id.src_mac_id = src_id;
	pmsg->id.dest_mac_id = dest_id;
	pmsg->id.source_id = 0;
	pmsg->id.func_id = ICAN_FUC_SENDTO;
	
	pmsg->id.ack = 1;
	pmsg->offset = 0;
	pmsg->src_cfg = 0;
	pmsg->len = 0;
	pmsg->frame_data_ptr = NULL;
		
	ret = ican_send_msg(pmsg);
	
	return ret;
}

/** 
* 接收ICAN消息，将向上提交给应用层. 
* 如消息需要应答，向源地址发送应答消息.
* @param[in]   无. 
* @param[out]  无.  
* @retval  无. 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
static void ican_recv_msg_porcess(void *parameter)
{
	ican_comm_t *p = (ican_comm_t *)parameter;
	uint32_t id=0;
	for( uint8_t i=0; i<ICAN_MSG_MAX; i++ ){
		if( rt_memcmp(&p->sub_recv[i].id,&id,sizeof(ican_fd_id_t) )!=0 ){
			if( p->sub_recv[i].recv_comp ){
				
//				if( p->sub_recv[i].id.func_id == ICAN_FUC_EVE_TRAGER )
//				{
//					ican_sub_recv_clr(parameter,&p->sub_recv[i].id);
//					icanc_log("event targer ican pack %08x process\r\n",p->sub_recv[i].id);
//				}
//				else
				{
					ican_fd_msg_t msg;
					msg.id = p->sub_recv[i].id;
					msg.data_ptr = p->sub_recv[i].buff;
					msg.data_len = p->sub_recv[i].length;
					if( ican_mq_in(parameter,&msg) == RT_EOK ){
						
						//向应用层提交数据成功，并返回应答
						if( msg.id.dest_mac_id != ICAN_BROADCAST_ADDR ){
							switch( msg.id.func_id ){
								case ICAN_FUC_SENDTO:
								{
									if( msg.id.ack==0 ){
										ican_replay_ack(msg.id.src_mac_id, msg.id.dest_mac_id, ICAN_FUC_SENDTO);
									}
									break;
								}
							}
						}
						
						rt_memset(&(p->sub_recv[i]),0,sizeof(ican_sub_recv_t));
						LOG_I("other func ican pack %08x entry app queue successful",p->sub_recv[i].id);
					}
					else{
						//向应用层发送数据的队列已满，释放内存
						rt_free(p->sub_recv[i].buff);
						rt_memset(&(p->sub_recv[i]),0,sizeof(ican_sub_recv_t));
						LOG_I("other func ican pack %08x entry app queue failed",p->sub_recv[i].id);
					}
				}
				
				p->sub_recv[i].recv_comp = 0;
			}
			//超时，数据包丢弃
			else if( p->sub_recv[i].timeout++ > ICAN_SEG_TIMEOUT ){
				rt_free(p->sub_recv[i].buff);
				rt_memset(&(p->sub_recv[i]),0,sizeof(ican_sub_recv_t));
				LOG_I("ican pack %08x timeout delete",p->sub_recv[i].id);
			}
		}
	}
}	

/** 
* 发送到目标站. 
* 当timeout大于0时，等待目标站返回应答. 
* @param[in]   dest_id:目标地址，src_id：源地址，len:发送长度,buff:发送指针，timeout：应答超时时间. 
* @param[out]  无.  
* @retval  RT_EOK或RT_ERROR 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
int ican_sendto(uint8_t dest_id, uint8_t src_id, uint16_t len, uint8_t *buff, uint16_t timeout)
{
	INT8U ret = 0;
	ican_fd_frame_send_t * pmsg,ican_send;
	
	rt_memset(&ican_send, 0, sizeof(ican_fd_frame_send_t));
	
	pmsg = &ican_send;
	
	pmsg->id.src_mac_id = src_id;
	pmsg->id.dest_mac_id = dest_id;
	pmsg->id.source_id = 0;
	pmsg->id.func_id = ICAN_FUC_SENDTO;
	
	pmsg->id.ack = 0;
	pmsg->offset = 0;
	pmsg->src_cfg = 0;
	pmsg->len = len;
	pmsg->frame_data_ptr = buff;
		
	ret = ican_send_msg(pmsg);
	
	if(ret == ICAN_OK){
		
		if( timeout>0 ){
			uint8_t data[8];
			uint16_t msg_len;
			ican_fd_id_t id={0},mask={0};
			
			id.dest_mac_id = src_id;
			id.src_mac_id = dest_id;
			id.func_id = ICAN_FUC_SENDTO;
			id.ack = 0x01;
			
			mask.dest_mac_id = 0xff;
			mask.src_mac_id = 0xff;
			mask.func_id = 0xf;
			mask.ack = 0x01;
			
			int ret = ican_read_msg(&id, &mask, data, 8, timeout, &msg_len);
			if( ret == ICAN_MSG_OK ){
				LOG_I("ican sendto msg OK");
				return RT_EOK;
			}
			else if( ret == ICAN_MSG_TIMEOUT ){
				LOG_I("ican sendto msg FAILED");
				return RT_ERROR;
			}
		}
		else{
			LOG_I("ican sendto msg OK not wait ack");
			return RT_EOK;
		}
	}
	else{
		LOG_I("maybe the ican send queue is full");
		return RT_EFULL;
	}
}
RTM_EXPORT(ican_sendto);

/** 
* 接收目标地址与源地址相同目标站数据. 
* 无.
* @param[in]   dest_id:目标地址，src_id：源地址，len:接收长度,buff:接收指针，timeout：等待超时时间. 
* @param[out]  无.  
* @retval  返回接收长度 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
int ican_recvfrom(uint8_t dest_id, uint8_t src_id, uint16_t len,uint8_t *buff, uint16_t timeout)
{
	uint8_t data[8];
	uint16_t msg_len;
	ican_fd_id_t id={0},mask={0};
	
	id.dest_mac_id = dest_id;
	id.src_mac_id = src_id;
	id.source_id = 0;
	id.func_id = ICAN_FUC_SENDTO;
	
	mask.dest_mac_id = 0xff;
	mask.src_mac_id = 0x00;
	mask.source_id = 0x00;
	mask.func_id = 0xf;
	if( ican_read_msg(&id, &mask, buff, len, timeout, &msg_len) == ICAN_MSG_OK ){
		return msg_len;
	}
	else{
		return 0;
	}
}
RTM_EXPORT(ican_recvfrom);

/** 
* ican轮询接收及发送线程. 
* 无.
* @param[in]   pdata:线程参数. 
* @param[out]  无.  
* @retval  无 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
void ican_comm_task(void * parameter)
{	
	ican_comm_t *p = (ican_comm_t *)parameter;
	while(1){
		
		ican_recv_msg(parameter);
		ican_recv_msg_porcess(parameter);
		
		//<接收can驱动数据入接收队列
		can_read_recv(&p->can_port);
		//<将发送队列数据写入can驱动
		can_write_send(&p->can_port);

		rt_thread_delay(5);
	}
}

/** 
* ican通讯初始化. 
* 建立应用提交队列及建立线程.
* @param[in]   parameter:传入的句柄. 
* @param[out]  无.  
* @retval  无 
* @par 标识符 
*      保留 
* @par 其它 
*      无 
* @par 修改日志 
*      ken deng于2020-05-26创建 
*/
rt_err_t ican_comm_init(void *parameter)
{
	ican_comm_t *p = (ican_comm_t *)parameter;
	rt_err_t result;
	result = rt_mq_init(&p->ican_mq,
                        "ican_mqt",
                        p->msg_pool,             				/* 内存池指向 msg_pool */
                        sizeof(struct _ican_fd_msg),            	/* 每个消息的大小是 1 字节 */
                        (RT_ALIGN(sizeof(ican_fd_msg_t), RT_ALIGN_SIZE)+4)*ICAN_MSG_MAX,	/* 内存池的大小是 msg_pool 的大小 */
                        RT_IPC_FLAG_FIFO);					  		/* 如果有多个线程等待，按照先来先得到的方法分配消息 */
						
	if( result != RT_EOK )
		return RT_ERROR;
	
	p_ican_app_mq = &p->ican_mq;
	
	rt_thread_t thread=RT_NULL;
	
	thread = rt_thread_create("ican_comm", ican_comm_task, parameter, 1024, 5, 10);
	
	if (thread != RT_NULL){
		
		rt_thread_startup(thread);
	}
	else{
		LOG_E("Can not create ican_comm thread");
		return RT_ERROR;
	}
}