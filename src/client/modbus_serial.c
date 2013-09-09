#include "modbus_serial.h"
#include "common_lib.h"

static void serial_timer_exp(void *com);
static unsigned char serial_rsp_received(void *com,
	unsigned char *data,
	unsigned char cnt);
static unsigned char serial_send_complete(void *proxy);

extern struct modbus_serial *modbus_serial_create(
	void *proxy,
	unsigned char mode)
{
	struct modbus_serial *link = (struct modbus_serial *)malloc(sizeof(*link));
	link->proxy = proxy;
	link->pdu_buf = 0;
	link->slave = 0;
	link->com = modbus_com_create(mode);
	modbus_com_set_callback(
		link->com,
		serial_send_complete,
		serial_rsp_received);
	link->ms = MASTER_IDLE;
	return link;
}

extern unsigned char modbus_serial_response_complete(struct modbus_serial *link)
{
	if (link->pdu_rsp != 0) {
		free(link->pdu_rsp->data);
		free(link->pdu_rsp);
		link->pdu_rsp = 0;
	}
	if (link->ms == MASTER_PROC_REPLY
		|| link->ms == MASTER_PROC_ERROR) {
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
			common_start_timer(
				&(link->turnard_timer),
				serial_timer_exp, link);
		} else {
			link->wait_timer = XXX; //[TODO:wait time is to be determined.]
			link->ms = MASTER_WAIT_REPLY;
			common_start_timer(
				&(link->wait_timer),
				serial_timer_exp, link);
		}
		link->pdu_req = pdu;
		serial_send_req_start(
			link->com,
			link->slave,
			link->pdu_req);
	}
}

extern unsigned char modbus_serial_set_handler(
	struct modbus_serial *link,
	unsigned char (*serial_send_complete)(
		void *proxy),
	unsigned char (*serial_rsp_received)(
		void *proxy,
		struct modbus_pdu *pdu))
{
	link->serial_send_complete = serial_send_complete;
	link->serial_rsp_received = serial_rsp_received;
}

static void serial_timer_exp(void *com)
{
	struct modbus_serial *link = data;
	if (MASTER_WAIT_TURNARD == link->ms) {
		if (link->turnard_timer != 0) {
			link->turnard_timer++;
			if (0 == link->turnard_timer) {
				link->ms = MASTER_IDLE;
			}
		}
	}
	if (MASTER_WAIT_REPLY == link->ms) {
		if (link->wait_timer != 0) {
			link->wait_timer++;
			if (0 == link->wait_timer) {
				link->ms = MASTER_PROC_ERROR;
			}
		}
	}
}

static unsigned char serial_rsp_received(void *com,
	unsigned char *data,
	unsigned char cnt)
{
	struct modbus_serial *link = com;
	if (MASTER_WAIT_REPLY == link->ms) {
		if (0 == cnt) {
			link->ms = MASTER_PROC_ERROR;
		} else {
			struct modbus_pdu *pdu =
				(struct modbus_pdu *)malloc(sizeof(*pdu));
			if (link->slave == data[0]) {
				pdu->opt = data[1];
				pdu->data = data + 2;
				pdu->count = cnt - 4;
				link->wait_timer = 0;
				link->ms = MASTER_PROC_REPLY;
				link->serial_rsp_received(
					link->proxy,
					pdu);
			}
		}
	}
}
static unsigned char serial_send_complete(
	void *com)
{
	struct modbus_serial *link = com;
	link->serial_send_complete(link->proxy);
}

