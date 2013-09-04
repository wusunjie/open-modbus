#ifndef _MODBUS_APPLI_H
#define _MODBUS_APPLI_H

#include "modbus_proxy.h"

struct modbus_appli {
	struct modbus_proxy *proxy;
	struct modbus_pdu *pdu_buf;
};

struct modbus_read_coils_req {
	unsigned short addr;
	unsigned short cnt;
};

struct modbus_appli_handler {
	unsigned char opt;
	unsigned char (*rsp_received)(
		unsigned char *data,
		unsigned char cnt);
	unsigned char (*exp_received)(
		unsigned char exp_code);
};

extern struct modbus_appli *modbus_appli_create(
	struct modbus_appli_handler *handler,
	unsigned char mode);

extern unsigned char modbus_appli_read_coils(
	struct modbus_appli *appli,
	struct modbus_read_coils_req *req);

#endif
