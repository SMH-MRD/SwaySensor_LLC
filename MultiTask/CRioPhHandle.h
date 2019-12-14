#pragma once

#include ".\src\ModbusTCPLibMain.h"

#define RIO_ERR_ITEM_INIT_FAIL				0x0001	//RIO初期化失敗

//RIOエラービット
#define RIO_ERR_ITEM_INIT_FAIL				0x0001	//RIO初期化失敗
#define RIO_ERR_TYPE_PARAM_SET				0x0003	//初期化パラメータセットエラー応答
#define RIO_ERR_TYPE_AI_READ1				0x0004	//データ読み込みエラー応答
#define RIO_ERR_TYPE_AI_READ2				0x0006	//データ読み込みエラー応答

//RIOセットパラメータ
#define RIO_COMMAND_REGISTER				2006	// COMMAND REGISTER 
#define RIO_COMMAND_AI_PORT_ACTIVE			0x8000	// 2006書き込みで2100-2279への設定値を有効にする
#define RIO_PORT_REGISTER_PORT1_MODE		2120	// PORT1のmode設定レジスタ 2006書き込みで有効となる
#define RIO_PORT_REGISTER_PORT2_MODE		2140	// PORT2のmode設定レジスタ 2006書き込みで有効となる
#define RIO_PORT_REGISTER_MODE_IOLINK		0x0004	// 2120,2140への設定値　アナログユニットはIO LINKモード04とする
#define RIO_PORT1_IN_ADDRESS				8002	// PORT1読み込みデータアドレス
#define RIO_PORT2_IN_ADDRESS				8018	// PORT2読み込みデータアドレス 
#define RIO_PORT1_OUT_ADDRESS				9002	// PORT1読み込みデータアドレス
#define RIO_PORT2_OUT_ADDRESS				9018	// PORT2読み込みデータアドレス 
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
	ST_MODBUS_TCP_REQ	stModbusTcpReq_p1read;//PORT1読み込み用要求設定
	ST_MODBUS_TCP_REQ	stModbusTcpReq_p2read;//PORT2読み込み用要求設定
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

