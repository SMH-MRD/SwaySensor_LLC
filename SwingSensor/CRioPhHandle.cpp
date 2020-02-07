#include "stdafx.h"
#include "CRioPhHandle.h"
#include "SharedObjects.h"

/************************************/
/* 構造体定義						*/
/************************************/
typedef struct _stRIORegTable {
	UINT portMode;
	UINT inAddr;
	UINT outAddr;
}stRIORegTable;

/************************************/
/* 変数定義							*/
/************************************/
ST_RioPh CRioPhHandle::stRIO_ph;
static CSharedData* m_cSharedData;
static const stRIORegTable m_stRegTable[RIO_PORT_NUM + 1] = {
	{0, 0, 0},
	{RIO_PORT_REGISTER_PORT1_MODE, RIO_PORT1_IN_ADDRESS, RIO_PORT1_OUT_ADDRESS},
	{RIO_PORT_REGISTER_PORT2_MODE, RIO_PORT2_IN_ADDRESS, RIO_PORT2_OUT_ADDRESS},
	{RIO_PORT_REGISTER_PORT3_MODE, RIO_PORT3_IN_ADDRESS, RIO_PORT3_OUT_ADDRESS},
	{RIO_PORT_REGISTER_PORT4_MODE, RIO_PORT4_IN_ADDRESS, RIO_PORT4_OUT_ADDRESS},
	{RIO_PORT_REGISTER_PORT5_MODE, RIO_PORT5_IN_ADDRESS, RIO_PORT5_OUT_ADDRESS},
	{RIO_PORT_REGISTER_PORT6_MODE, RIO_PORT6_IN_ADDRESS, RIO_PORT6_OUT_ADDRESS},
	{RIO_PORT_REGISTER_PORT7_MODE, RIO_PORT7_IN_ADDRESS, RIO_PORT7_OUT_ADDRESS},
	{RIO_PORT_REGISTER_PORT8_MODE, RIO_PORT8_IN_ADDRESS, RIO_PORT8_OUT_ADDRESS},
};

CRioPhHandle::CRioPhHandle(){
	stRIO_ph.bRIO_init_ok = false;
	m_cSharedData = new CSharedData();
	m_cSharedData->SetBevelData(BEVEL_ID_PORT_1_ANALOG, NAN);
	m_cSharedData->SetBevelData(BEVEL_ID_PORT_1_MA, NAN);
	m_cSharedData->SetBevelData(BEVEL_ID_PORT_2_ANALOG, NAN);
	m_cSharedData->SetBevelData(BEVEL_ID_PORT_2_MA, NAN);
}
CRioPhHandle::~CRioPhHandle(){}

unsigned __stdcall CRioPhHandle::RioPhThread(void *pVoid) {

	if (stRIO_ph.bRIO_init_ok == false) {
		if (!init_RIO()) {
			stRIO_ph.bRIO_init_ok = true;
			stRIO_ph.error_status = 0x0000;
		}
	}
	else {
		//PORT1データ読み込み
		stRIO_ph.error_code = modtGetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq_p1read, (uint8_t *)stRIO_ph.RIO_ai_port1.uint8);

		if (stRIO_ph.error_code) {
			stRIO_ph.error_status = RIO_ERR_TYPE_AI_READ1;
			stRIO_ph.bRIO_init_ok = false;
			m_cSharedData->SetBevelData(BEVEL_ID_PORT_1_ANALOG, NAN);
			m_cSharedData->SetBevelData(BEVEL_ID_PORT_1_MA, NAN);
		}
		else {
			//PORT1読み込みデータ mA変換
			if (stRIO_ph.RIO_ai_port1.uint16 == 0x7FFF) stRIO_ph.RIO_ai_p1_mA = 22.81;
			else if (stRIO_ph.RIO_ai_port1.uint16 == 0x8000) stRIO_ph.RIO_ai_p1_mA = 1.186;
			else stRIO_ph.RIO_ai_p1_mA = 4.0 + 16.0 / 65536.0 * (double)(stRIO_ph.RIO_ai_port1.uint16);
			m_cSharedData->SetBevelData(BEVEL_ID_PORT_1_ANALOG, (double)stRIO_ph.RIO_ai_port1.uint16);
			m_cSharedData->SetBevelData(BEVEL_ID_PORT_1_MA, stRIO_ph.RIO_ai_p1_mA);
		}
	
		//PORT2データ読み込み
		stRIO_ph.error_code = modtGetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq_p2read, (uint8_t *)stRIO_ph.RIO_ai_port2.uint8);

		if (stRIO_ph.error_code) {
			stRIO_ph.error_status = RIO_ERR_TYPE_AI_READ2;
			stRIO_ph.bRIO_init_ok = false;
			m_cSharedData->SetBevelData(BEVEL_ID_PORT_2_ANALOG, NAN);
			m_cSharedData->SetBevelData(BEVEL_ID_PORT_2_MA, NAN);
		}
		else {
			//PORT2読み込みデータ mA変換
			if (stRIO_ph.RIO_ai_port2.uint16 == 0x7FFF) stRIO_ph.RIO_ai_p2_mA = 22.81;
			else if (stRIO_ph.RIO_ai_port2.uint16 == 0x8000) stRIO_ph.RIO_ai_p2_mA = 1.186;
			else stRIO_ph.RIO_ai_p2_mA = 4.0 + 16.0 / 65536.0 * (double)(stRIO_ph.RIO_ai_port2.uint16);
			m_cSharedData->SetBevelData(BEVEL_ID_PORT_2_ANALOG, (double)stRIO_ph.RIO_ai_port2.uint16);
			m_cSharedData->SetBevelData(BEVEL_ID_PORT_2_MA, stRIO_ph.RIO_ai_p2_mA);
		}
	}

	return 0;
};

int CRioPhHandle::init_RIO() {
	string ipAddr;
	m_cSharedData->GetParam(PARAM_ID_STR_RIO_IPADDR, &ipAddr);
	memcpy(stRIO_ph.ip_string, ipAddr.c_str(), ipAddr.length());
	m_cSharedData->GetParam(PARAM_ID_RIO_TCPPORT, (UINT32*)&stRIO_ph.port_num);
	m_cSharedData->GetParam(PARAM_ID_RIO_SLAVEADDR, (UINT32*)&stRIO_ph.slave_addr);
	m_cSharedData->GetParam(PARAM_ID_RIO_TIMEOUT, (UINT32*)&stRIO_ph.timeOut);

	UINT32 xPort, yPort;
	m_cSharedData->GetParam(PARAM_ID_RIO_XPORT, &xPort);
	m_cSharedData->GetParam(PARAM_ID_RIO_YPORT, &yPort);

	stRIO_ph.modbusDesc = modtInit((const int8_t*)stRIO_ph.ip_string, stRIO_ph.port_num, stRIO_ph.timeOut);
	if (stRIO_ph.modbusDesc == NULL) {
		stRIO_ph.error_status = RIO_ERR_ITEM_INIT_FAIL;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}
	//PORT1をIO LINK MODEに設定
	stRIO_ph.stModbusTcpReq.slaveAddr = stRIO_ph.slave_addr;
	stRIO_ph.stModbusTcpReq.funcCode = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
	stRIO_ph.stModbusTcpReq.regAddr = m_stRegTable[xPort].portMode;
	stRIO_ph.stModbusTcpReq.dataCnt = 1;
	stRIO_ph.stModbusTcpReq.option = 0;
	stRIO_ph.setData[0].uint16 = RIO_PORT_REGISTER_MODE_IOLINK;

	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}

	//PORT2をIO LINK MODEに設定
//	stRIO_ph.stModbusTcpReq.regAddr = RIO_PORT_REGISTER_PORT2_MODE;
	stRIO_ph.stModbusTcpReq.regAddr = m_stRegTable[yPort].portMode;
	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}

	//パラメータ設定値有効化
	stRIO_ph.stModbusTcpReq.regAddr = RIO_COMMAND_REGISTER;
	stRIO_ph.stModbusTcpReq.dataCnt = 2;
	stRIO_ph.stModbusTcpReq.option = 0;
	stRIO_ph.setData[0].uint16 = RIO_COMMAND_AI_PORT_ACTIVE;

	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}

	//PORT1 AIのパラメータ設定
	stRIO_ph.stModbusTcpReq.funcCode = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;
	stRIO_ph.stModbusTcpReq.regAddr = m_stRegTable[xPort].outAddr;
	stRIO_ph.stModbusTcpReq.dataCnt = 1;
	stRIO_ph.stModbusTcpReq.option = 0;
	stRIO_ph.setData[0].uint16 = RIO_COMMAND_AI_PARA_SET;

	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}

	//PORT2 AIのパラメータ設定
//	stRIO_ph.stModbusTcpReq.regAddr = RIO_PORT2_OUT_ADDRESS;
	stRIO_ph.stModbusTcpReq.regAddr = m_stRegTable[yPort].outAddr;
	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}
		
	//PORT1読み込み設定登録
	stRIO_ph.stModbusTcpReq_p1read.slaveAddr = stRIO_ph.slave_addr;
	stRIO_ph.stModbusTcpReq_p1read.funcCode = MODBUS_TCPLIB_FUNCCODE_READ_REGISTER;
	stRIO_ph.stModbusTcpReq_p1read.regAddr = m_stRegTable[xPort].inAddr;
	stRIO_ph.stModbusTcpReq_p1read.dataCnt = 1;
	stRIO_ph.stModbusTcpReq_p1read.option = 0;

	//PORT2読み込み設定登録
	stRIO_ph.stModbusTcpReq_p2read.slaveAddr = stRIO_ph.slave_addr;
	stRIO_ph.stModbusTcpReq_p2read.funcCode = MODBUS_TCPLIB_FUNCCODE_READ_REGISTER;
//	stRIO_ph.stModbusTcpReq_p2read.regAddr = RIO_PORT2_IN_ADDRESS;
	stRIO_ph.stModbusTcpReq_p2read.regAddr = m_stRegTable[yPort].inAddr;
	stRIO_ph.stModbusTcpReq_p2read.dataCnt = 1;
	stRIO_ph.stModbusTcpReq_p2read.option = 0;


	return 0;
}
