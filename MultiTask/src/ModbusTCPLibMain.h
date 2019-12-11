#pragma once
#include <stdint.h>
#include <windows.h>

//******************************************************************************
// �萔��`
//******************************************************************************
/* �t�@���N�V�����R�[�h */
#define MODBUS_TCPLIB_FUNCCODE_READ_BIT					0x01	/* �t�@���N�V�����R�[�h:bit�Ǎ��� */
#define MODBUS_TCPLIB_FUNCCODE_READ_INPUT_BIT			0x02	/* �t�@���N�V�����R�[�h:����bit�Ǎ��� */
#define MODBUS_TCPLIB_FUNCCODE_READ_REGISTER			0x03	/* �t�@���N�V�����R�[�h:���W�X�^�Ǎ��� */
#define MODBUS_TCPLIB_FUNCCODE_READ_INPUT_REGISTER		0x04	/* �t�@���N�V�����R�[�h:���̓��W�X�^�Ǎ��� */
#define MODBUS_TCPLIB_FUNCCODE_WRITE_BIT				0x05	/* �t�@���N�V�����R�[�h:bit������ */
#define MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER			0x06	/* �t�@���N�V�����R�[�h:���W�X�^������ */
#define MODBUS_TCPLIB_FUNCCODE_WRITE_MULTI_BITS			0x0F	/* �t�@���N�V�����R�[�h:����bit������ */
#define MODBUS_TCPLIB_FUNCCODE_WRITE_MULTI_REGISTERS	0x10	/* �t�@���N�V�����R�[�h:�������W�X�^������ */

/* �߂�l�R�[�h */
#define MODBUS_TCPLIB_RET_OK			0			/* �߂�l:���� */
#define MODBUS_TCPLIB_RET_NG_INVALID	-1			/* �߂�l:�ُ�(��������) */
#define MODBUS_TCPLIB_RET_NG_TIMEOUT	-2			/* �߂�l:�ُ�(�^�C���A�E�g) */
#define MODBUS_TCPLIB_RET_NG_SYSERR		-3			/* �߂�l:�ُ�(�V�X�e���ُ�) */

/* �X���[�u�A�h���X */
#define MODBUS_TCPLIB_SLAVEADDR_MAX		247			/* �X���[�u�A�h���X�ő� */
#define MODBUS_TCPLIB_SLAVEADDR_SP		255			/* �X���[�u�A�h���X���� */

//******************************************************************************
// �^��`
//******************************************************************************
typedef void* ModbusTCPDesc;	// MODBUS TCP�f�B�X�N���v�^�\����

typedef struct _stModbusTCPReq {
	uint32_t slaveAddr;		// �X���[�u�A�h���X
	uint32_t funcCode;		// �t�@���N�V�����R�[�h
	DWORD option;			// �I�v�V����
	uint32_t regAddr;		// ���W�X�^�A�h���X
	uint32_t dataCnt;		// �f�[�^��
} ST_MODBUS_TCP_REQ;

//******************************************************************************
// �֐���`
//******************************************************************************
ModbusTCPDesc __stdcall modtInit(const int8_t* ip, int32_t port, uint32_t timeoutMSec);
int32_t __stdcall modtGetdata(ModbusTCPDesc modbusDesc, ST_MODBUS_TCP_REQ stModbusTcpReq, uint8_t* getData);
int32_t __stdcall modtSetdata(ModbusTCPDesc modbusDesc, ST_MODBUS_TCP_REQ stModbusTcpReq, uint8_t* setData);
int32_t __stdcall modtClose(ModbusTCPDesc modbusDesc);
