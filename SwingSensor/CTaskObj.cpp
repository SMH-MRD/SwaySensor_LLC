#pragma once

#include "stdafx.h"
#include "CTaskObj.h"

#include <windowsx.h>
#include <commctrl.h> //コモンコントロール用


using namespace std;

//# マルチスレッド基本クラス メンバー関数 

CTaskObj::CTaskObj() {//コンストラクタ
	inf.hndl = NULL; 
	inf.cycle_ms = DEFAUT_TASK_CYCLE;
	inf.thread_com = REPEAT_INFINIT;
	wsprintf(inf.name, L"Who am I?"); 
	inf.act_count = inf.act_time = 0;
	inf.priority = THREAD_PRIORITY_NORMAL;
	inf.work_select = THREAD_WORK_IDLE;
	inf.hWnd_parent = inf.hWnd_work = inf.hWnd_opepane = inf.hWnd_msgStatics = NULL;
	inf.hBmp = NULL;
};

CTaskObj::~CTaskObj() {
	delete inf.hBmp;
};//デストラクタ

void CTaskObj::init_task(void *pobj) {
	set_panel_tip_txt();  
	return; 
};

//# スレッドメイン関数
unsigned CTaskObj::run(void *param) {

	while (this->inf.thread_com != TERMINATE_THREAD){
		inf.event_triggered=WaitForMultipleObjects(inf.n_active_events,((CTaskObj*)param)->inf.hevents,FALSE, INFINITE);//メインスレッドからのSIGNAL状態待ち

		//処理周期確認用
		DWORD start_time = timeGetTime();
		inf.period = start_time - inf.start_time;
		inf.start_time = start_time;

		if (inf.work_select == THREAD_WORK_ROUTINE) routine_work(param);
		else if (inf.work_select == THREAD_WORK_OPTION1) optional_work1(param);
		else if (inf.work_select == THREAD_WORK_OPTION2) optional_work2(param);
		else default_work(param);//実処理

		inf.act_time = timeGetTime() - start_time;//実処理時間測定用
		if (this->inf.thread_com != REPEAT_INFINIT) break;//逐次処理の場合は抜ける
	}
	return 0;
}

void CTaskObj::routine_work(void *param) {

};


void CTaskObj::optional_work1(void *param) {

};
void CTaskObj::optional_work2(void *param) {

};

//# メインウィンドウへのショートメッセージ表示　string
void CTaskObj::tweet2owner(const std::string &src) {

};

//# メインウィンドウへのショートメッセージ表示　wstring
void CTaskObj::tweet2owner(const std::wstring &srcw) {

}; //メインウィンドウへのショートメッセージ表示

void CTaskObj::txout2msg_listbox(std::wstring str) {

};
void CTaskObj::txout2msg_listbox(std::string str) {

};

LRESULT CALLBACK CTaskObj::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {
	return 0;
};
HWND CTaskObj::CreateOwnWindow(HWND h_parent_wnd) {
	return inf.hWnd_work;
};

void CTaskObj::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {

}
void CTaskObj::set_panel_tip_txt()
{

}

