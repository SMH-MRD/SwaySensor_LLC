#pragma once
#include "CTaskObj.h"


extern vector<void*>	VectpCTaskObj;	//タスクオブジェクトのポインタ
extern ST_iTask g_itask;


#define COMD_INCL_AXIS_X	1
#define COMD_INCL_AXIS_Y	2
#define COMD_INCL_MODE_RAD	1
#define COMD_INCL_MODE_DEG	2


// dcom.ini関連
#define RIO_SECT_OF_INIFILE				L"RIO"			//RIOセクション
#define RIO_IP_KEY_OF_INIFILE			L"RIO_IP"		
#define RIO_PORT_KEY_OF_INIFILE			L"RIO_PORT"
#define RIO_TIMEOUT_KEY_OF_INIFILE		L"RIO_TMOUT"
#define RIO_TIMEOUT_KEY_OF_SLAVE		L"RIO_SLAVE_ADDR"


class CComDevice :
	public CTaskObj
{
public:
	CComDevice();
	~CComDevice();
	
	WCHAR path_of_inifile[_MAX_PATH];

	double inclination_rad_x;
	double inclination_rad_y;
	double inclination_deg_x;
	double inclination_deg_y;

	void routine_work(void *param);
	void init_task(void *pobj);

	int set_RIO_incl();
	double get_RIO_incl(int mode, int axis);
	
	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	void set_panel_tip_txt();//タブパネルのStaticテキストを設定

};

