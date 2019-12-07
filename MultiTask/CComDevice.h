#pragma once
#include "CTaskObj.h"


extern vector<void*>	VectpCTaskObj;	//タスクオブジェクトのポインタ
extern ST_iTask g_itask;


#define COMD_INCL_AXIS_X	1
#define COMD_INCL_AXIS_Y	2
#define COMD_INCL_MODE_RAD	1
#define COMD_INCL_MODE_DEG	2


typedef struct _stRIO {
	DWORD error_status;
	double inclination_rad_x;
	double inclination_rad_y;
	double inclination_deg_x;
	double inclination_deg_y;
}ST_RIO, *LPST_RIO;


class CComDevice :
	public CTaskObj
{
public:
	CComDevice();
	~CComDevice();
	
	ST_RIO s_RIO;

	void routine_work(void *param);
	void init_task(void *pobj);

	LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	void set_panel_tip_txt();//タブパネルのStaticテキストを設定
		
	int set_RIO_incl();
	double get_RIO_incl(int mode, int axis);
};

