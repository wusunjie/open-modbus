#ifndef _MODBUS_SERIAL_H
#define _MODBUS_SERIAL_H

#include "modbus_proxy.h"
#include "modbus_com.h"

struct modbus_serial {
	unsigned char (*serial_send_complete)(
		struct modbus_proxy *proxy);
	unsigned char (*serial_rsp_received)(
		struct modbus_proxy *proxy,
		struct modbus_pdu *pdu);
	struct modbus_com *com;
	struct modbus_proxy *proxy;
	struct modbus_pdu *pdu_rsp;
	struct modbus_pdu *pdu_req;
	unsigned char slave;
	enum master_status ms;
	unsigned int turnard_timer;
	unsigned int wait_timer;
};

extern struct modbus_serial *modbus_serial_create(
	struct modbus_proxy *proxy,
	unsigned char mode);

extern unsigned char modbus_serial_response_complete(struct modbus_serial *link);

extern unsigned char modbus_serial_send_rq(
	struct modbus_serial *link,
	struct modbus_pdu *pdu,
	unsigned char slave);

extern unsigned char modbus_serial_set_handler(
	unsigned char (*serial_send_complete)(
		struct modbus_proxy *proxy),
	unsigned char (*serial_rsp_received)(
		struct modbus_proxy *proxy,
		struct modbus_pdu *pdu))

extern void modbus_serial_2ms_task(struct modbus_serial *link);

#endif
