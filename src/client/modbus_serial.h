#ifndef _MODBUS_SERIAL_H
#define _MODBUS_SERIAL_H

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
		unsigned char *pdu,
		unsigned char cnt);
	struct modbus_com *com;
	void *proxy;
	unsigned char *pdu_rsp;
	unsigned char pdu_rsp_cnt;
	unsigned char pdu_req_opt;
	unsigned char *pdu_req;
	unsigned char pdu_req_cnt;
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
	unsigned char pdu_opt,
	unsigned char *pdu_data,
	unsigned char pdu_cnt,
	unsigned char slave);

extern unsigned char modbus_serial_set_handler(
	unsigned char (*serial_send_complete)(
		void *proxy),
	unsigned char (*serial_rsp_received)(
		void *proxy,
		unsigned char *pdu,
		unsigned char cnt))
#endif
