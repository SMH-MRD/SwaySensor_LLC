#include "stdafx.h"
#include "CComDeviceRIO.h"
#include "Helper.h"
#include "CRioPhHandle.h"

CRioPhHandle* cRioHandle;
CComDeviceRIO::CComDeviceRIO() { cRioHandle = new CRioPhHandle(); }
CComDeviceRIO::~CComDeviceRIO(){}

void CComDeviceRIO::init_task(void *pobj) {

	//タスクタブウィンドウの表示文字設定
	set_panel_tip_txt();
	CRioPhHandle::stRIO_ph.bRIO_init_ok = false;
	
	return;
};

void CComDeviceRIO::routine_work(void *param) {
	
	cRioHandle->RioPhRead();
};

LRESULT CALLBACK CComDeviceRIO::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {
	return 0;
};

void CComDeviceRIO::set_panel_tip_txt()
{

}
