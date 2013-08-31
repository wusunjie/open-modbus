#ifndef _MODBUS_H
#define _MODBUS_H

#include "common_lib.h"

struct modbus_pdu {
	unsigned char  opt;
	unsigned char *data;
	unsigned char count;
};

struct modbus_excep_pdu {
	unsigned char opt;
	unsigned char excep_code;
};

struct modbus_rsp_handler {
	unsigned char (*rsp_received)(struct modbus_pdu *pdu);
	unsigned char (*excep_handler)(struct modbus_excep_pdu *pdu);
};

struct modbus_proxy {
	common_queue_t *request;
};

extern unsigned char modbus_request_send(
	struct modbus_proxy *proxy,
	struct modbus_pdu *request,
	struct modbus_rsp_handler *handler
);

#endif
