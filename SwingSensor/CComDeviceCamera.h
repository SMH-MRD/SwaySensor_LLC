#pragma once
#include "CTaskObj.h"


extern vector<void*>	VectpCTaskObj;	//タスクオブジェクトのポインタ
extern ST_iTask g_itask;

class CComDeviceCamera :
	public CTaskObj
{
public:
	CComDeviceCamera();
	~CComDeviceCamera();

	void routine_work(void* param);
	void init_task(void* pobj);

	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	void set_panel_tip_txt();//タブパネルのStaticテキストを設定

};

