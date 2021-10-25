
#include "ican_convert.h"
#include "rtdevice.h"
#include "rtthread.h"
#include "ican_port.h"

static INT32S msg_convert(rt_can_msg_t pmsg, ican_fd_frame_t * pframe, INT8U mode)
{
	if((pmsg == NULL) || (pframe == NULL) || ((mode != ICAN_BXCAN) && (mode != BXCAN_ICAN)))
		return RT_EINVAL;

	if(mode == ICAN_BXCAN)
	{
		pmsg->id = (pframe->id.source_id & 0xff) | ((pframe->id.func_id & 0x0f) << 8) | ((pframe->id.ack & 0x01) << 12) |
				   ((pframe->id.dest_mac_id & 0xff) << 13) | ((pframe->id.src_mac_id & 0xff) << 21);
		
		pmsg->len = pframe->dlc;
		pmsg->ide = RT_CAN_EXTID;
		rt_memcpy(pmsg->data, pframe->frame_data, pmsg->len);
//		pmsg->is_exid = IS_EXID;
//		pmsg->is_rtr = NO_RTR;
	}
	else if(mode == BXCAN_ICAN)
	{
		pframe->id.source_id = pmsg->id & 0xff;
		pframe->id.func_id = (pmsg->id >> 8) & 0x0f;
		pframe->id.ack = (pmsg->id >> 12) & 0x01;
		pframe->id.dest_mac_id = (pmsg->id >> 13) & 0xff;
		pframe->id.src_mac_id = (pmsg->id >> 21) & 0xff;

		pframe->dlc = pmsg->len;
		rt_memcpy(pframe->frame_data, pmsg->data, pframe->dlc);
	}
	else
		return RT_EINVAL;

	return RT_EOK;
}

rt_err_t ican_send(ican_fd_frame_t * pframe)
{
	struct rt_can_msg msg = {0};
//	rt_memset(&msg, 0, sizeof(struct rt_can_msg));
	msg_convert(&msg, pframe, ICAN_BXCAN);
	return can_write(&msg);
}

rt_err_t ican_recv(ican_fd_frame_t * pframe)
{
	struct rt_can_msg msg = {0};
	rt_memset(&msg, 0, sizeof(struct rt_can_msg));
	if( can_read(&msg) == RT_EOK ){
		msg_convert(&msg, pframe, BXCAN_ICAN);
		return RT_EOK;
	}
	else{
		return RT_ERROR;
	}
}

INT8U ican_get_device_addr(void)
{
	 return 0x01;
}
