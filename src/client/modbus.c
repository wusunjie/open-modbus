#include "modbus.h"

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

