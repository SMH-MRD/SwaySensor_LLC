#pragma once
#include "CTaskObj.h"

extern vector<void*>	VectpCTaskObj;	//�^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask g_itask;

class CAnalyst :
	public CTaskObj
{
public:
	CAnalyst();
	~CAnalyst();

	void routine_work(void* param);
	void init_task(void* pobj);
	void ImageProc(void);
	void BevelProc(void);
};

