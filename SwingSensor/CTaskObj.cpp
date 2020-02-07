#pragma once

#include "stdafx.h"
#include "CTaskObj.h"

#include <windowsx.h>
#include <commctrl.h> //�R�����R���g���[���p


using namespace std;

//# �}���`�X���b�h��{�N���X �����o�[�֐� 

CTaskObj::CTaskObj() {//�R���X�g���N�^
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
};//�f�X�g���N�^

void CTaskObj::init_task(void *pobj) {
	set_panel_tip_txt();  
	return; 
};

//# �X���b�h���C���֐�
unsigned CTaskObj::run(void *param) {

	while (this->inf.thread_com != TERMINATE_THREAD){
		inf.event_triggered=WaitForMultipleObjects(inf.n_active_events,((CTaskObj*)param)->inf.hevents,FALSE, INFINITE);//���C���X���b�h�����SIGNAL��ԑ҂�

		//���������m�F�p
		DWORD start_time = timeGetTime();
		inf.period = start_time - inf.start_time;
		inf.start_time = start_time;

		if (inf.work_select == THREAD_WORK_ROUTINE) routine_work(param);
		else if (inf.work_select == THREAD_WORK_OPTION1) optional_work1(param);
		else if (inf.work_select == THREAD_WORK_OPTION2) optional_work2(param);
		else default_work(param);//������

		inf.act_time = timeGetTime() - start_time;//���������ԑ���p
		if (this->inf.thread_com != REPEAT_INFINIT) break;//���������̏ꍇ�͔�����
	}
	return 0;
}

void CTaskObj::routine_work(void *param) {

};


void CTaskObj::optional_work1(void *param) {

};
void CTaskObj::optional_work2(void *param) {

};

//# ���C���E�B���h�E�ւ̃V���[�g���b�Z�[�W�\���@string
void CTaskObj::tweet2owner(const std::string &src) {

};

//# ���C���E�B���h�E�ւ̃V���[�g���b�Z�[�W�\���@wstring
void CTaskObj::tweet2owner(const std::wstring &srcw) {

}; //���C���E�B���h�E�ւ̃V���[�g���b�Z�[�W�\��

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

