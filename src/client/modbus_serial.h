#ifndef _MODBUS_SERIAL_H
#define _MODBUS_SERIAL_H

#include "modbus_proxy.h"
#include "modbus_com.h"

enum master_status {
	MASTER_IDLE,
	MASTER_PROC_REPLY,
	MASTER_PROC_ERROR,
	MASTER_WAIT_TURNARD,
	MASTER_WAIT_REPLY	
}

struct modbus_serial {
	unsigned char (*serial_send_complete)(
		struct modbus_proxy *proxy);
	unsigned char (*serial_rsp_received)(
		struct modbus_proxy *proxy,
		struct modbus_pdu *pdu);
	struct modbus_com *com;
	void *proxy;
	struct modbus_pdu *pdu_rsp;
	struct modbus_pdu *pdu_req;
	unsigned char slave;
	enum master_status ms;
	unsigned int turnard_timer;
	unsigned int wait_timer;
};

extern struct modbus_serial *modbus_serial_create(
	void *proxy,
	unsigned char mode);

extern unsigned char modbus_serial_response_complete(struct modbus_serial *link);

extern unsigned char modbus_serial_send_rq(
	struct modbus_serial *link,
	struct modbus_pdu *pdu,
	unsigned char slave);

extern unsigned char modbus_serial_set_handler(
	unsigned char (*serial_send_complete)(
		void *proxy),
	unsigned char (*serial_rsp_received)(
		void *proxy,
		struct modbus_pdu *pdu))
#endif
