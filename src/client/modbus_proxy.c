#include "modbus.h"
#include "common_lib.h"

enum modbus_rq_status {
	RQ_STATUS_WAIT,
	RQ_STATUS_PENDING
};

struct modbus_request {
	enum modbus_rq_status status;
	struct modbus_pdu *rq_pdu;
	struct modbus_rsp_handler *handler;
};

extern unsigned char modbus_request_send(
	struct modbus_proxy *proxy,
	struct modbus_pdu *request,
	struct modbus_handler *handler
)
{
	struct modbus_request *rq = (struct modbus_request *)malloc(sizeof(*rq));
	rq->status = RQ_STATUS_WAIT;
	rq->rq_pdu = request;
	rq->handler = handler;
	common_push_queue(proxy->request, rq);
}

