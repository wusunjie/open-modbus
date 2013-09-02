#ifndef _MODBUS_APPLI_H
#define _MODBUS_APPLI_H

struct modbus_appli {
	struct modbus_proxy *proxy;
	unsigned char *pdu_buf;
};

struct modbus_read_coils_req {
	unsigned short addr;
	unsigned short cnt;
};

typedef (*modbus_appli_handler)(unsigned char *data, unsigned char cnt);

extern struct modbus_appli *modbus_appli_create(modbus_appli_handler *handler);

extern unsigned char modbus_appli_read_coils(
	struct modbus_appli *appli,
	struct modbus_read_coils_req *req);

#endif
