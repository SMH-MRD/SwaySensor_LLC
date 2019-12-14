#pragma once

#include ".\src\ModbusTCPLibMain.h"

#define RIO_ERR_ITEM_INIT_FAIL				0x0001	//RIO���������s

//RIO�G���[�r�b�g
#define RIO_ERR_ITEM_INIT_FAIL				0x0001	//RIO���������s
#define RIO_ERR_TYPE_PARAM_SET				0x0003	//�������p�����[�^�Z�b�g�G���[����
#define RIO_ERR_TYPE_AI_READ1				0x0004	//�f�[�^�ǂݍ��݃G���[����
#define RIO_ERR_TYPE_AI_READ2				0x0006	//�f�[�^�ǂݍ��݃G���[����

//RIO�Z�b�g�p�����[�^
#define RIO_COMMAND_REGISTER				2006	// COMMAND REGISTER 
#define RIO_COMMAND_AI_PORT_ACTIVE			0x8000	// 2006�������݂�2100-2279�ւ̐ݒ�l��L���ɂ���
#define RIO_PORT_REGISTER_PORT1_MODE		2120	// PORT1��mode�ݒ背�W�X�^ 2006�������݂ŗL���ƂȂ�
#define RIO_PORT_REGISTER_PORT2_MODE		2140	// PORT2��mode�ݒ背�W�X�^ 2006�������݂ŗL���ƂȂ�
#define RIO_PORT_REGISTER_MODE_IOLINK		0x0004	// 2120,2140�ւ̐ݒ�l�@�A�i���O���j�b�g��IO LINK���[�h04�Ƃ���
#define RIO_PORT1_IN_ADDRESS				8002	// PORT1�ǂݍ��݃f�[�^�A�h���X
#define RIO_PORT2_IN_ADDRESS				8018	// PORT2�ǂݍ��݃f�[�^�A�h���X 
#define RIO_PORT1_OUT_ADDRESS				9002	// PORT1�ǂݍ��݃f�[�^�A�h���X
#define RIO_PORT2_OUT_ADDRESS				9018	// PORT2�ǂݍ��݃f�[�^�A�h���X 
#define RIO_COMMAND_AI_PARA_SET				0x8010	// AI: 15bit set command  8,9bit filter 4,5 format->0 sampling filter, S7 format


#define RIO_PORT_NUM		8

#define ADDR_HIGH			1
#define ADDR_LOW			0

union UNION_WandB{
	uint16_t uint16;	
	uint8_t uint8[2];
};

typedef struct _stRioPh {

	ModbusTCPDesc		modbusDesc;
	ST_MODBUS_TCP_REQ	stModbusTcpReq;
	ST_MODBUS_TCP_REQ	stModbusTcpReq_p1read;//PORT1�ǂݍ��ݗp�v���ݒ�
	ST_MODBUS_TCP_REQ	stModbusTcpReq_p2read;//PORT2�ǂݍ��ݗp�v���ݒ�
	UNION_WandB setData[2];

	bool  bRIO_init_ok;
	int32_t error_status;
	int32_t error_code;

	char	ip_string[20U];
	int32_t	port_num;
	int32_t timeOut;
	int32_t slave_addr;

	UNION_WandB RIO_ai_port1;
	UNION_WandB RIO_ai_port2;

	double RIO_ai_p1_mA;
	double RIO_ai_p2_mA;

}ST_RioPh, *LPST_RioPh;

class CRioPhHandle
{
public:
	CRioPhHandle();
	~CRioPhHandle();

	static ST_RioPh stRIO_ph;
	static unsigned __stdcall RioPhThread(void *pVoid);
	static int init_RIO();
	static int read_AI();

};

