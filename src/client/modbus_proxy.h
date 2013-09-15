#ifndef _MODBUS_PROXY_H
#define _MODBUS_PROXY_H


#include "common_lib.h"
#include "modbus_serial.h"

struct modbus_pdu {
	unsigned char  opt;
	unsigned char *data;
	unsigned char count;
};

typedef (*rsp_received_cb)(
	void *appli,
	unsigned char *pdu,
	unsigned char pdu_cnt,
	unsigned char exp);

struct modbus_proxy {
	common_queue_t *request;
	common_queue_t *response;
	struct modbus_serial *link;
	void *appli;
	rsp_received_cb rsp_received;
	unsigned char (*send_complete_notify)(void *appli);
	unsigned char slave;
};

extern struct modbus_proxy *modbus_proxy_create(rsp_received_cb cb);

extern unsigned char modbus_proxy_send_rq(
	struct modbus_proxy *proxy,
	struct modbus_pdu *request,
	unsigned char slave);

extern unsigned char modbus_response_complete(struct modbus_proxy *proxy);

extern unsigned char modbus_set_received_cb(
	struct modbus_proxy *proxy,
	rsp_received_cb cb);

#endif
