#pragma once

//RIOエラービット
#define RIO_ERR_ITEM_INIT_FAIL				0x0100	//RIO初期化失敗
#define RIO_ERR_TYPE_NULL_DESC				0x0001	//初期化関数エラー応答
#define RIO_ERR_TYPE_PARAM_SET				0x0002	//パラメータ設定不可

//RIOセットパラメータ
#define RIO_COMMAND_REGISTER				2006	//
#define RIO_COMMAND_REGISTER_PORT_CONFIG	0x8000	//
#define RIO_PORT_REGISTER_PORT1_MODE		2120	//
#define RIO_PORT_REGISTER_PORT2_MODE		2140	//
#define RIO_PORT_REGISTER_MODE_IOLINK		0x0004	//



typedef struct _stRioPh {
	bool  bRIO_init_ok;
	DWORD error_status;

	char	ip_string[20U];
	int32_t	port_num;
	int32_t timeOut;
	DWORD RIO_ai_port1;
	DWORD RIO_ai_port2;
}ST_RioPh, *LPST_RioPh;

class CRioPhHandle
{
public:
	CRioPhHandle();
	~CRioPhHandle();

	static ST_RioPh sRIO_ph;
	static unsigned __stdcall RioPhThread(void *pVoid);
	static int init_RIO();

};

