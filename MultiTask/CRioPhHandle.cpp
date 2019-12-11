#include "stdafx.h"
#include "CRioPhHandle.h"
#include ".\src\ModbusTCPLibMain.h"


CRioPhHandle::CRioPhHandle(){
	sRIO_ph.bRIO_init_ok = false;
}
CRioPhHandle::~CRioPhHandle(){}

ST_RioPh CRioPhHandle::sRIO_ph;

unsigned __stdcall CRioPhHandle::RioPhThread(void *pVoid) {
	if (sRIO_ph.bRIO_init_ok == false) {
		init_RIO();
	}
	else {
		;
	}
	return 0;
};

int CRioPhHandle::init_RIO() {
	if (!sRIO_ph.bRIO_init_ok) {
		ModbusTCPDesc modbusDesc = modtInit((const int8_t*)sRIO_ph.ip_string, sRIO_ph.port_num, sRIO_ph.timeOut);
		if (modbusDesc == NULL) {
			sRIO_ph.error_status |= RIO_ERR_ITEM_INIT_FAIL;
			return -1;
		}
		else sRIO_ph.bRIO_init_ok = true;
	}
	return 0;
}
