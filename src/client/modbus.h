#ifndef _MODBUS_H
#define _MODBUS_H

struct read_discrete_inputs_cb {
	unsigned char (*response)(
		unsigned char byte_cnt,
		unsigned char *coil_status
	);
	unsigned char (*error)(
		unsigned char error_code
	);
};

extern unsigned char modbus_read_discrete_inputs_req(
	unsigned short int addr,
	unsigned short int coil_cnt,
	struct read_discrete_inputs_cb *cb
);

#endif
