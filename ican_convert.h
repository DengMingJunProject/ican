#ifndef __ICAN_CONVERT_H__
#define __ICAN_CONVERT_H__

#include "ican.h"
#include "ican_port.h"

#define ICAN_BXCAN			0
#define BXCAN_ICAN			1

rt_err_t ican_send(ican_fd_frame_t * pframe);
rt_err_t ican_recv(ican_fd_frame_t * pframe);
INT8U ican_get_device_addr(void);

#endif
