#ifndef _MODBUS_H
#define _MODBUS_H

#include "common_lib.h"
#include "modbus_serial.h"
#include "modbus_appli.h"

struct modbus_pdu {
	unsigned char  opt;
	unsigned char *data;
	unsigned char count;
};

typedef (*rsp_received_cb)(
	struct modbus_appli *appli,
	struct modbus_pdu *pdu,
	unsigned char exp);

struct modbus_proxy {
	common_queue_t *request;
	common_queue_t *response;
	struct modbus_serial *link;
	struct modbus_appli *appli;
	rsp_received_cb rsp_received;
	unsigned char (*send_complete_notify)(struct modbus_appli *appli);
};

extern unsigned char modbus_request_send(
	struct modbus_proxy *proxy,
	struct modbus_pdu *request,
);

extern unsigned char modbus_response_complete(struct modbus_proxy *proxy);

#endif
