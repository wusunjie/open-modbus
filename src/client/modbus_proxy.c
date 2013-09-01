#include "modbus.h"
#include "common_lib.h"

enum modbus_rq_status {
	RQ_STATUS_WAIT,
	RQ_STATUS_PENDING
};

struct modbus_request {
	enum modbus_rq_status status;
	struct modbus_pdu *rq_pdu;
};

static unsigned char serial_send_complete(struct modbus_proxy *proxy);
static unsigned char serial_resp_received(
	struct modbus_proxy *proxy,
	struct modbus_pdu *pdu);

extern unsigned char modbus_request_send(
	struct modbus_proxy *proxy,
	struct modbus_pdu *request,
	rsp_received_cb handler)
{
	struct modbus_request *rq =
		(struct modbus_request *)malloc(sizeof(*rq));
	rq->status = RQ_STATUS_WAIT;
	rq->rq_pdu = request;
	proxy->rsp_received = handler;
	common_push_queue(proxy->request, rq);
}

extern unsigned char modbus_response_complete(struct modbus_proxy *proxy)
{
	struct modbus_request *rsp = common_head_queue(proxy->response);
	unsigned char exp = 0;
	if (RQ_STATUS_PENDING == rsp->status) {
		proxy->response_complete_notify(proxy->link);
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
			exp
		)) {
			rsp->status = RQ_STATUS_PENDING;
		}
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
		if (1 == modbus_serial_send_rq(proxy->link, req->pdu)) {
			req->status = RQ_STATUS_PENDING;
		}
	}
}

static unsigned char serial_resp_received(
	struct modbus_proxy *proxy,
	struct modbus_pdu *pdu)
{
	struct modbus_request *rsp =
		(struct modbus_request *)malloc(sizeof(*rsp));
	rsp->status = RQ_STATUS_WAIT;
	rsp->rq_pdu = pdu;
	common_push_queue(proxy->response);
}

