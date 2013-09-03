#include "modbus.h"
#include "common_lib.h"
#include "modbus_serial.h"

enum modbus_rq_status {
	RQ_STATUS_WAIT,
	RQ_STATUS_PENDING
};

struct modbus_request {
	enum modbus_rq_status status;
	struct modbus_pdu *rq_pdu;
	common_queue_t *queue;
};

static unsigned char serial_send_complete(struct modbus_proxy *proxy);
static unsigned char serial_rsp_received(
	struct modbus_proxy *proxy,
	struct modbus_pdu *pdu);

extern struct modbus_proxy *modbus_proxy_create(
	struct modbus_appli *appli,
	unsigned char (*send_complete_notify_cb)(
		struct modbus_appli *))
{
	struct modbus_proxy *proxy = (struct modbus_proxy *)malloc(sizeof(*proxy));
	proxy->appli = appli;
	proxy->link = modbus_serial_create(proxy);
	proxy->send_complete_notify = send_complete_notify_cb;
	proxy->slave = 0;
	modbus_serial_set_handler(
		proxy->link,
		serial_send_complete,
		serial_rsp_received);
	return proxy;
}

extern unsigned char modbus_proxy_send_rq(
	struct modbus_proxy *proxy,
	struct modbus_pdu *request,
	unsigned char slave)
{
	struct modbus_request *rq =
		(struct modbus_request *)malloc(sizeof(*rq));
	rq->status = RQ_STATUS_WAIT;
	rq->rq_pdu = request;
	proxy->slave = slave;
	common_push_queue(proxy->request, rq);
}

extern unsigned char modbus_response_complete(struct modbus_proxy *proxy)
{
	struct modbus_request *rsp = common_head_queue(proxy->response);
	unsigned char exp = 0;
	if (RQ_STATUS_PENDING == rsp->status) {
		modbus_serial_response_complete(proxy->link);
		common_pop_queue(rsp);
		free(rsp);
		rsp = common_head_queue(proxy->response);
	}

	if (rsp != 0) {
		if ((rsp->rq_pdu[0] & 0x80) != 0) {
			exp = 1;
		} else {
			exp = 0;
		}
		if (1 == proxy->rsp_received(
			proxy->appli,
			rsp->rq_pdu,
			exp)) {
			rsp->status = RQ_STATUS_PENDING;
		}
	}
}

extern unsigned char modbus_set_received_cb(
	struct modbus_proxy *proxy,
	rsp_received_cb cb)
{
	if (cb != 0) {
		proxy->rsp_received = cb;
	}
}

static unsigned char serial_send_complete(struct modbus_proxy *proxy)
{
	struct modbus_request *req = common_head_queue(proxy->request);
	if (RQ_STATUS_PENDING == req->status) {
		proxy->send_complete_notify(proxy->appli);
		common_pop_queue(req);
		free(req);
		req = common_head_queue(proxy->request);
	}

	if (req != 0) {
		if (1 == modbus_serial_send_rq(proxy->link, req->pdu, proxy->slave)) {
			req->status = RQ_STATUS_PENDING;
		}
	}
}

static unsigned char serial_rsp_received(
	struct modbus_proxy *proxy,
	struct modbus_pdu *pdu)
{
	struct modbus_request *rsp =
		(struct modbus_request *)malloc(sizeof(*rsp));
	rsp->status = RQ_STATUS_WAIT;
	rsp->rq_pdu = pdu;
	common_push_queue(proxy->response);
}

