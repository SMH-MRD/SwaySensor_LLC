#include "stdafx.h"
#include "CRioPhHandle.h"


CRioPhHandle::CRioPhHandle(){
	stRIO_ph.bRIO_init_ok = false;
}
CRioPhHandle::~CRioPhHandle(){}

ST_RioPh CRioPhHandle::stRIO_ph;

unsigned __stdcall CRioPhHandle::RioPhThread(void *pVoid) {
	ModbusTCPDesc modbusDes;

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
			return -1;
		}
	
		//PORT2データ読み込み
		stRIO_ph.error_code = modtGetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq_p2read, (uint8_t *)stRIO_ph.RIO_ai_port2.uint8);

		if (stRIO_ph.error_code) {
			stRIO_ph.error_status = RIO_ERR_TYPE_AI_READ2;
			return -1;
		}

		//PORT1読み込みデータ mA変換
		if (stRIO_ph.RIO_ai_port1.uint16 == 0x7FFF) stRIO_ph.RIO_ai_p1_mA = 22.81;
		else if (stRIO_ph.RIO_ai_port1.uint16 == 0x8000) stRIO_ph.RIO_ai_p1_mA = 1.186;
		else {
			stRIO_ph.RIO_ai_port1.uint16 &= 0x7FF8;
			stRIO_ph.RIO_ai_p1_mA = 4.0 + 16.0 / 30000.0 * (double)(stRIO_ph.RIO_ai_port1.uint16);
		}

		//PORT21読み込みデータ mA変換
		if (stRIO_ph.RIO_ai_port2.uint16 == 0x7FFF) stRIO_ph.RIO_ai_p2_mA = 22.81;
		else if (stRIO_ph.RIO_ai_port2.uint16 == 0x8000) stRIO_ph.RIO_ai_p2_mA = 1.186;
		else {
			stRIO_ph.RIO_ai_port2.uint16 &= 0x7FF8;
			stRIO_ph.RIO_ai_p2_mA = 4.0 + 16.0 / 30000.0 * (double)(stRIO_ph.RIO_ai_port2.uint16);
		}
	}


	return 0;
};

int CRioPhHandle::init_RIO() {

	stRIO_ph.modbusDesc = modtInit((const int8_t*)stRIO_ph.ip_string, stRIO_ph.port_num, stRIO_ph.timeOut);
	if (stRIO_ph.modbusDesc == NULL) {
		stRIO_ph.error_status = RIO_ERR_ITEM_INIT_FAIL;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}
	//PORT1をIO LINK MODEに設定
	stRIO_ph.stModbusTcpReq.slaveAddr = stRIO_ph.slave_addr;					//RIOのリンクアドレス
	stRIO_ph.stModbusTcpReq.funcCode = MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER;	//Modbus　ファンクションコード
	stRIO_ph.stModbusTcpReq.regAddr = RIO_PORT_REGISTER_PORT1_MODE;				//アクセスレジスタ
	stRIO_ph.stModbusTcpReq.dataCnt = 1;										//アクセスレジスタ数
	stRIO_ph.stModbusTcpReq.option = 0;
	stRIO_ph.setData[0].uint16 = RIO_PORT_REGISTER_MODE_IOLINK;

	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}

	//PORT2をIO LINK MODEに設定
	stRIO_ph.stModbusTcpReq.regAddr = RIO_PORT_REGISTER_PORT2_MODE;
	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}

	//PORT1のIN DATA LENGTH設定
	stRIO_ph.stModbusTcpReq.regAddr = RIO_PORT_REGISTER_PORT1_IN_LENGTH;
	stRIO_ph.stModbusTcpReq.dataCnt = 2;
	stRIO_ph.setData[0].uint16 = 0x0002;
	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}

	//PORT1のOUT DATA LENGTH設定
	stRIO_ph.stModbusTcpReq.regAddr = RIO_PORT_REGISTER_PORT1_OUT_LENGTH;
	stRIO_ph.stModbusTcpReq.dataCnt = 2;
	stRIO_ph.setData[0].uint16 = 0x0002;
	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}

	//PORT2のIN DATA LENGTH設定
	stRIO_ph.stModbusTcpReq.regAddr = RIO_PORT_REGISTER_PORT2_IN_LENGTH;
	stRIO_ph.stModbusTcpReq.dataCnt = 2;
	stRIO_ph.setData[0].uint16 = 0x0002;
	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}

	//PORT2のOUT DATA LENGTH設定
	stRIO_ph.stModbusTcpReq.regAddr = RIO_PORT_REGISTER_PORT2_OUT_LENGTH;
	stRIO_ph.stModbusTcpReq.dataCnt = 2;
	stRIO_ph.setData[0].uint16 = 0x0002;
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
	stRIO_ph.stModbusTcpReq.regAddr = RIO_PORT1_OUT_ADDRESS;
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
	stRIO_ph.stModbusTcpReq.regAddr = RIO_PORT2_OUT_ADDRESS;
	stRIO_ph.error_code = modtSetdata(stRIO_ph.modbusDesc, stRIO_ph.stModbusTcpReq, stRIO_ph.setData[0].uint8);
	if (stRIO_ph.error_code) {
		stRIO_ph.error_status = RIO_ERR_TYPE_PARAM_SET;
		stRIO_ph.bRIO_init_ok = false;
		return -1;
	}
		
	//PORT1読み込み設定登録
	stRIO_ph.stModbusTcpReq_p1read.slaveAddr = stRIO_ph.slave_addr;
	stRIO_ph.stModbusTcpReq_p1read.funcCode = MODBUS_TCPLIB_FUNCCODE_READ_REGISTER;
	stRIO_ph.stModbusTcpReq_p1read.regAddr = RIO_PORT1_IN_ADDRESS;
	stRIO_ph.stModbusTcpReq_p1read.dataCnt = 1;
	stRIO_ph.stModbusTcpReq_p1read.option = 0;

	//PORT2読み込み設定登録
	stRIO_ph.stModbusTcpReq_p2read.slaveAddr = stRIO_ph.slave_addr;
	stRIO_ph.stModbusTcpReq_p2read.funcCode = MODBUS_TCPLIB_FUNCCODE_READ_REGISTER;
	stRIO_ph.stModbusTcpReq_p2read.regAddr = RIO_PORT2_IN_ADDRESS;
	stRIO_ph.stModbusTcpReq_p2read.dataCnt = 1;
	stRIO_ph.stModbusTcpReq_p2read.option = 0;


	return 0;
}
