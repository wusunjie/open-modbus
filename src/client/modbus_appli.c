#include "modbus_appli.h"
#include "modbus_proxy.h"
#include "common_lib.h"

static modbus_appli_handler *appli_handler = 0;

static unsigned char appli_send_complete(struct modbus_appli *appli);
static unsigned char appli_rsp_received(
	struct modbus_appli *appli,
	struct modbus_pdu *pdu,
	unsigned char exp);

extern struct modbus_appli *modbus_appli_create(modbus_appli_handler *table)
{
	struct modbus_appli *appli = (struct modbus_appli *)malloc(sizeof(*appli));
	appli->proxy = modbus_proxy_create(appli, appli_send_complete);
	appli->pdu_buf = 0;
	modbus_set_received_cb(appli->proxy, appli_rsp_received);
	appli_handler = table;
	return appli;
}

extern unsigned char modbus_appli_read_coils(
	struct modbus_appli *appli,
	struct modbus_read_coils_req *req)
{
	appli->pdu_buf = (struct modbus_pdu *)malloc(sizeof(*(appli->pdu_buf)));
	appli->pdu_buf->opt = XXX;
	appli->pdu_buf->data = (unsigned char *)malloc(4);
	appli->pdu_buf->data[0] = (unsigned char)(req->addr >> 8);
	appli->pdu_buf->data[1] = (unsigned char)(req->addr & 0xFF);
	appli->pdu_buf->data[2] = (unsigned char)(req->cnt >> 8);
	appli->pdu_buf->data[3] = (unsigned char)(req->cnt & 0xFF);
	appli->pdu_buf->count = 4;
	modbus_proxy_send_rq(appli->proxy, appli->pdu_buf);
}

static unsigned char appli_send_complete(struct modbus_appli *appli)
{
	if (appli->pdu_buf != 0) {
		free(appli->pdu_buf->data);
		free(appli->pdu_buf);
		appli->pdu_buf = 0;
	}
}

static unsigned char appli_rsp_received(
	struct modbus_appli *appli,
	struct modbus_pdu *pdu,
	unsigned char exp)
{
	appli_handler[pdu->opt](pdu->data, pdu->count, exp);
}
