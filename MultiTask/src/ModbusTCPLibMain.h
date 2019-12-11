#pragma once
#include <stdint.h>
#include <windows.h>

//******************************************************************************
// 定数定義
//******************************************************************************
/* ファンクションコード */
#define MODBUS_TCPLIB_FUNCCODE_READ_BIT					0x01	/* ファンクションコード:bit読込み */
#define MODBUS_TCPLIB_FUNCCODE_READ_INPUT_BIT			0x02	/* ファンクションコード:入力bit読込み */
#define MODBUS_TCPLIB_FUNCCODE_READ_REGISTER			0x03	/* ファンクションコード:レジスタ読込み */
#define MODBUS_TCPLIB_FUNCCODE_READ_INPUT_REGISTER		0x04	/* ファンクションコード:入力レジスタ読込み */
#define MODBUS_TCPLIB_FUNCCODE_WRITE_BIT				0x05	/* ファンクションコード:bit書込み */
#define MODBUS_TCPLIB_FUNCCODE_WRITE_REGISTER			0x06	/* ファンクションコード:レジスタ書込み */
#define MODBUS_TCPLIB_FUNCCODE_WRITE_MULTI_BITS			0x0F	/* ファンクションコード:複数bit書込み */
#define MODBUS_TCPLIB_FUNCCODE_WRITE_MULTI_REGISTERS	0x10	/* ファンクションコード:複数レジスタ書込み */

/* 戻り値コード */
#define MODBUS_TCPLIB_RET_OK			0			/* 戻り値:正常 */
#define MODBUS_TCPLIB_RET_NG_INVALID	-1			/* 戻り値:異常(無効引数) */
#define MODBUS_TCPLIB_RET_NG_TIMEOUT	-2			/* 戻り値:異常(タイムアウト) */
#define MODBUS_TCPLIB_RET_NG_SYSERR		-3			/* 戻り値:異常(システム異常) */

/* スレーブアドレス */
#define MODBUS_TCPLIB_SLAVEADDR_MAX		247			/* スレーブアドレス最大 */
#define MODBUS_TCPLIB_SLAVEADDR_SP		255			/* スレーブアドレス特殊 */

//******************************************************************************
// 型定義
//******************************************************************************
typedef void* ModbusTCPDesc;	// MODBUS TCPディスクリプタ構造体

typedef struct _stModbusTCPReq {
	uint32_t slaveAddr;		// スレーブアドレス
	uint32_t funcCode;		// ファンクションコード
	DWORD option;			// オプション
	uint32_t regAddr;		// レジスタアドレス
	uint32_t dataCnt;		// データ数
} ST_MODBUS_TCP_REQ;

//******************************************************************************
// 関数定義
//******************************************************************************
ModbusTCPDesc __stdcall modtInit(const int8_t* ip, int32_t port, uint32_t timeoutMSec);
int32_t __stdcall modtGetdata(ModbusTCPDesc modbusDesc, ST_MODBUS_TCP_REQ stModbusTcpReq, uint8_t* getData);
int32_t __stdcall modtSetdata(ModbusTCPDesc modbusDesc, ST_MODBUS_TCP_REQ stModbusTcpReq, uint8_t* setData);
int32_t __stdcall modtClose(ModbusTCPDesc modbusDesc);
