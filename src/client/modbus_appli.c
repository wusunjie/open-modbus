#include "modbus_appli.h"
#include "modbus_proxy.h"
#include "common_lib.h"

static struct modbus_appli_handler *appli_handler = 0;

static const unsigned char READ_DISCRETE_INPUTS	= 0x02;
static const unsigned char READ_COILS		= 0x01;
static const unsigned char WRITE_SINGLE_COIL	= 0x05;
static const unsigned char WRITE_MULTI_COILS	= 0x0F;
static const unsigned char READ_INPUT_REG	= 0x04;
static const unsigned char READ_HOLDING_REG	= 0x03;
static const unsigned char WRITE_SINGLE_REG	= 0x06;
static const unsigned char WRITE_MULTI_REG	= 0x10;
static const unsigned char RW_MULTI_REG		= 0x17;
static const unsigned char MASK_WRITE_REG	= 0x16;
static const unsigned char READ_FIFO_QUEUE	= 0x18;
static const unsigned char READ_FILE_RECORD	= 0x14;
static const unsigned char WRITE_FILE_RECORD	= 0x15;
static const unsigned char READ_EXCEPT_STATUS	= 0x07;
static const unsigned char DIAGNOSTIC		= 0x08;
static const unsigned char GET_COM_EVENT_CNT	= 0x11;
static const unsigned char GET_COM_EVENT_LOG	= 0x12;
static const unsigned char REPORT_SERVER_ID	= 0x17;
static const unsigned char READ_DEVICE_ID	= 0x43;
static const unsigned char MODBUS_OPT_NULL	= 0xFF;


static unsigned char appli_send_complete(struct modbus_appli *appli);
static unsigned char appli_rsp_received(
	struct modbus_appli *appli,
	struct modbus_pdu *pdu,
	unsigned char exp);

extern struct modbus_appli *modbus_appli_create(struct modbus_appli_handler *table)
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
	unsigned char i;
	for (i = 0; appli_handler[i].opt != MODBUS_OPT_NULL; i++) {
		if (appli_handler[i].opt == pdu->opt) {
			break;
		}
	}
	if (appli_handler[i].opt != MODBUS_OPT_NULL) {
		if (exp == 0) {
			appli_handler[i].rsp_received(pdu->data, pdu->count);
		} else {
			appli_handler[i].exp_received(pdu->data[0]);
		}
	}
}

