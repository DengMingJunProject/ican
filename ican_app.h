
#ifndef __ICAN_APP_H__
#define __ICAN_APP_H__

#include "ican_comm.h"
#include "stdint.h"
#include "stdbool.h"

typedef struct _ican_app
{
	ican_comm_t			ican_comm;
	
}ican_app_t;

extern int ican_app_init(void);

#endif
