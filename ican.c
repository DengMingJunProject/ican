#include "ican.h"

const ican_src_node_t ican_src_list[]=
{
	ICAN_IO_SRC,	0,	0x00,	0x1f,	ICAN_SRC_DI,
	ICAN_IO_SRC,	0,	0x20,	0x3f,	ICAN_SRC_DO,
	ICAN_IO_SRC,	0,	0x40,	0x5f,	ICAN_SRC_AI,
	ICAN_IO_SRC,	0,	0x60,	0x7f,	ICAN_SRC_AO,
	ICAN_IO_SRC,	0,	0x80,	0x9f,	ICAN_SRC_SERIAL0,
	ICAN_IO_SRC,	0,	0xa0,	0xbf,	ICAN_SRC_SERIAL1,
	ICAN_IO_SRC,	0,	0xc0,	0xdf,	ICAN_SRC_RESERVE,
	
	ICAN_CFG_SRC,	ICAN_CFG_DEVICE_INFO,	0xe0,	0xe1,	ICAN_VENDOR_ID,
	ICAN_CFG_SRC,	ICAN_CFG_DEVICE_INFO,	0xe2,	0xe3,	ICAN_PRODUCT_TYPE,
	ICAN_CFG_SRC,	ICAN_CFG_DEVICE_INFO,	0xe4,	0xe5,	ICAN_PRODUCT_CODE,
	ICAN_CFG_SRC,	ICAN_CFG_DEVICE_INFO,	0xe6,	0xe7,	ICAN_HARDWARE_VERSION,
	ICAN_CFG_SRC,	ICAN_CFG_DEVICE_INFO,	0xe8,	0xe9,	ICAN_FIRMWARE_VERSION,
	ICAN_CFG_SRC,	ICAN_CFG_DEVICE_INFO,	0xea,	0xed,	ICAN_SERIAL_NUMBER,
	
	ICAN_CFG_SRC,	ICAN_CFG_COMMUNICATE,	0xee,	0xee,	ICAN_MAC_ID,
	ICAN_CFG_SRC,	ICAN_CFG_COMMUNICATE,	0xef,	0xef,	ICAN_BAUDRATE,
	ICAN_CFG_SRC,	ICAN_CFG_COMMUNICATE,	0xf0,	0xf3,	ICAN_USER_BAUDRATE,
	ICAN_CFG_SRC,	ICAN_CFG_COMMUNICATE,	0xf4,	0xf4,	ICAN_CYCLIC_PARAM,
	ICAN_CFG_SRC,	ICAN_CFG_COMMUNICATE,	0xf5,	0xf5,	ICAN_CYCLIC_MASTER,
	ICAN_CFG_SRC,	ICAN_CFG_COMMUNICATE,	0xf6,	0xf6,	ICAN_COS_TYPE,
	ICAN_CFG_SRC,	ICAN_CFG_COMMUNICATE,	0xf7,	0xf7,	ICAN_MASTER_MAC_ID,
	
	ICAN_CFG_SRC,	ICAN_CFG_IO_PARAM,		0xf8,	0xf8,	ICAN_IO_PARAM,
	ICAN_CFG_SRC,	ICAN_CFG_IO_CFG,		0xf9,	0xf9,	ICAN_IO_CFG,
	ICAN_CFG_SRC,	ICAN_CFG_IO_RESERVE,	0xfa,	0xff,	ICAN_IO_RESERVE,
};

INT8U ican_get_src_id(uint8_t addr,uint8_t *len)
{
	ican_src_node_t const * src_node;
	src_node = &ican_src_list[0];
	for( uint8_t i=0; i<sizeof(ican_src_list)/sizeof(ican_src_node_t); i++ ){
		if( addr >= src_node->start_addr && addr <= src_node->end_addr ){
			return src_node->declare;
		}
	}
	return ICAN_ERR_SOURCE_ID;
}

INT8U ican_get_src_addr(uint8_t src_id, uint8_t offset)
{
	ican_src_node_t const *src_node;
	src_node = &ican_src_list[0];
	for( uint8_t i=0; i<sizeof(ican_src_list)/sizeof(ican_src_node_t); i++ ){
		if( src_node->declare == src_id ){
			return src_node->start_addr + offset;
		}
	}
	return ICAN_ERR_ADDR;
}

INT8U ican_get_src_type(uint8_t src_id)
{
	ican_src_node_t const *src_node;
	src_node = &ican_src_list[0];
	for( uint8_t i=0; i<sizeof(ican_src_list)/sizeof(ican_src_node_t); i++ ){
		if( src_node->declare == src_id ){
			return src_node->type;
		}
	}
	return 0;
}

INT8U ican_get_src_macid(ican_fd_frame_t * pframe)
{
	return (INT8U)(pframe->id.src_mac_id);
}

void ican_set_src_macid(ican_fd_frame_t * pframe, INT8U src_macid)
{
	pframe->id.src_mac_id = src_macid;
}

INT8U ican_get_dest_macid(ican_fd_frame_t * pframe)
{
	return (INT8U)(pframe->id.dest_mac_id);
}

void ican_set_dest_macid(ican_fd_frame_t * pframe, INT8U dest_macid)
{
	pframe->id.dest_mac_id = dest_macid;
}

INT8U ican_get_func_id(ican_fd_frame_t * pframe)
{
	return (INT8U)(pframe->id.func_id);
}

void ican_set_func_id(ican_fd_frame_t * pframe, INT8U func_id)
{
	pframe->id.func_id = func_id & 0x0f;
}

INT8U ican_get_source_id(ican_fd_frame_t * pframe)
{
	return (INT8U)(pframe->id.source_id);
}

void ican_set_source_id(ican_fd_frame_t * pframe, INT8U source_id)
{
	pframe->id.source_id = source_id;
}

INT8U ican_is_need_ack(ican_fd_frame_t * pframe)
{
	return (INT8U)(pframe->id.ack);
}

void ican_set_ack(ican_fd_frame_t * pframe, INT8U ack)
{
	pframe->id.ack = ack;
}

INT8U ican_get_split_flag(ican_fd_frame_t * pframe)
{
	return (INT8U)((pframe->frame_data[0] >> 6) & 0x03);
}

void ican_set_split_flag(ican_fd_frame_t * pframe, INT8U split_flag)
{
	pframe->frame_data[0] &= 0x3f;
	pframe->frame_data[0] |= (split_flag << 6);
}

INT8U ican_get_split_num(ican_fd_frame_t * pframe)
{
	return (INT8U)(pframe->frame_data[0] & 0x3f);
}

void ican_set_split_num(ican_fd_frame_t * pframe, INT8U split_num)
{
	pframe->frame_data[0] &= 0xc0;
	pframe->frame_data[0] |= (split_num & 0x3f);
}

void ican_clear_split_info(ican_fd_frame_t * pframe)
{
	pframe->frame_data[0] = 0x00;
}
INT8U ican_get_offset(ican_fd_frame_t * pframe)
{
	return pframe->frame_data[2];
}
void ican_set_offset(ican_fd_frame_t * pframe, INT8U offset)
{
	pframe->frame_data[2] = offset;
}
INT8U ican_get_length(ican_fd_frame_t * pframe)
{
	return pframe->frame_data[1];
}
void ican_set_length(ican_fd_frame_t * pframe, INT8U length)
{
	pframe->frame_data[1] = length;
}
