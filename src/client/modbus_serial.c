#include "modbus_serial.h"
#include "modbus_proxy.h"
#include "common_lib.h"

extern struct modbus_serial *modbus_serial_create(
	struct modbus_proxy *proxy,
	unsigned char mode)
{
	struct modbus_serial *link = (struct modbus_serial *)malloc(sizeof(*link));
	link->proxy = proxy;
	link->pdu_buf = 0;
	link->slave = 0;
	link->com = modbus_com_create(mode);
	return link;
}

extern unsigned char modbus_serial_response_complete(struct modbus_serial *link)
{
	if (link->pdu_rsp != 0) {
		free(link->pdu_rsp->data);
		free(link->pdu_rsp);
		link->pdu_rsp = 0;
	}
	if (link->ms == MASTER_PROC_REPLY) {
		link->ms = MASTER_IDLE;
	}
}

extern unsigned char modbus_send_rq(
	struct modbus_serial *link,
	struct modbus_pdu *pdu,
	unsigned char slave)
{
	if (link->ms == MASTER_IDLE) {
		link->pdu_req = pdu;	
		link->slave = slave;
		if (slave == 0) {
			link->turnard_timer = XXX; //[TODO:turn around time is to be determined.]
			link->ms = MASTER_WAIT_TURNARD;
		} else {
			link->wait_timer = XXX; //[TODO:wait time is to be determined.]
			link->ms = MASTER_WAIT_RESPONSE;
		}
		link->pdu_req = pdu;
		link->com->send_req_start(link->slave, link->pdu_req);
	}
}

extern unsigned char modbus_serial_set_handler(
	struct modbus_serial *link,
	unsigned char (*serial_send_complete)(
		struct modbus_proxy *proxy),
	unsigned char (*serial_rsp_received)(
		struct modbus_proxy *proxy,
		struct modbus_pdu *pdu))
{
	link->serial_send_complete = serial_send_complete;
	link->serial_rsp_received = serial_rsp_received;
}

extern void modbus_serial_2ms_task(struct modbus_serial *link)
{
	if (link->turnard_timer != 0) {
		link->turnard_timer++;
		if (0 == link->turnard_timer) {
			switch (link->ms) {
				case MASTER_WAIT_TURNARD:
				{
					link->ms = MASTER_IDLE;
				}
				break;
				default:
				break;
			}
		}
	}
	if (link->wait_timer != 0) {
		link->wait_timer++;
		if (0 == link->wait_timer) {
			switch (link->ms) {
				case MASTER_WAIT_REPLY:
				{
					link->ms = MASTER_PROC_ERROR;
				}
				break;
				default:
				break;
			}
		}
	}
}

