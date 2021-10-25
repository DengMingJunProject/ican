
#include "ican_app.h"
#include "ican.h"
#include "ican_convert.h"
#include "rtthread.h"
#include "string.h"
#include "ican_port.h"

static ican_app_t	ican_app;

void ican_max_send(int argc, char *argv[])
{
	if( argc==2 ){
	
		uint16_t len;
		len = atoi(argv[1]);
		len = len>ICAN_MAX_PACK?ICAN_MAX_PACK:len;
		if( len>0 ){
			uint8_t *ptr = rt_malloc(len);
			if( ptr ){
				for( uint16_t i=0; i<len; i++ ){
					ptr[i] = i;
				}
				if( ican_sendto(12, 0, len, ptr,150) == RT_EOK){
					rt_kprintf("send OK\r\n");
				}
				else{
					rt_kprintf("send failed \r\n");
				}
				rt_free(ptr);
			}
		}
	}
}
MSH_CMD_EXPORT(ican_max_send,"ican master max send[len]" );

static uint8_t recv_buf[100];
void ican_app_task(void *parameter)
{
	while(1)
	{
		uint16_t recv_len;
		recv_len = ican_recvfrom(12, 0, 100,recv_buf,10);
		if( recv_len ){
			for( uint16_t i=0; i<recv_len; i++ ){
				rt_kprintf("%02x ",recv_buf[i]);
			}
		}
		rt_thread_delay(10);
	}
}

int ican_app_init(void)
{
	struct rt_can_filter_item items[] =
    {
		//接收本地ID,将源ID也必需加入过滤表，否则收不到应答
		RT_CAN_FILTER_ITEM_INIT(ICAN_ID_ITEM_INIT(1,0,0,0,0),1,0,0,ICAN_ID_ITEM_INIT(0xff,0,0,0,0), RT_NULL, RT_NULL),
		RT_CAN_FILTER_ITEM_INIT(ICAN_ID_ITEM_INIT(0,1,0,0,0),1,0,0,ICAN_ID_ITEM_INIT(0,0xff,0,0,0), RT_NULL, RT_NULL),
		//接收ICAN广播数据
		RT_CAN_FILTER_ITEM_INIT(ICAN_ID_ITEM_INIT(0xff,0,0,0,0),1,0,0,ICAN_ID_ITEM_INIT(0xff,0,0,0,0), RT_NULL, RT_NULL),
    };
	
    struct rt_can_filter_config cfg = {3, 1, items}; /* 一共有 3 个过滤表 */
	can_init(&ican_app.ican_comm.can_port,"can1",CAN1MBaud, CAN1MBaud, &cfg);
	
	ican_comm_init(&ican_app.ican_comm);
	
	rt_thread_t thread=RT_NULL;
	thread = rt_thread_create("ican_app", ican_app_task, NULL, 1024, 15, 10);
	
	if (thread != RT_NULL){
		
		rt_thread_startup(thread);
	}
	else{
		return RT_ERROR;
	}
	
	return RT_EOK;
}
INIT_COMPONENT_EXPORT(ican_app_init);
