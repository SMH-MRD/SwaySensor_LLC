#include "stdafx.h"
#include "SharedObjects.h"

/************************************/
/* 構造体定義						*/
/************************************/
typedef struct _stMngImageData
{
	Mat image;
	BOOL update;
}STMngImageData;

typedef struct _stMngProcData
{
	STProcData data;
	BOOL update;
}STMngProcData;

typedef struct _stMngBevelData
{
	DOUBLE data;
}STMngBevelData;

/************************************/
/* 変数定義							*/
/************************************/
static STMngImageData m_stImage[IMAGE_ID_CAM_MAX];
static STMngProcData m_stProcImage[IMAGE_ID_PROC_MAX];
static STMngBevelData m_stBevelData[BEVEL_ID_MAX];
static UINT32 m_u32Param[PARAM_ID_MAX];
static string m_strParam[PARAM_ID_STR_MAX];
static DOUBLE m_dParam[PARAM_ID_DOUBLE_MAX];

static CRITICAL_SECTION csImage[IMAGE_ID_CAM_MAX];
static CRITICAL_SECTION csProcImage[IMAGE_ID_PROC_MAX];
static CRITICAL_SECTION csBevel[BEVEL_ID_MAX];
static CRITICAL_SECTION csParam[PARAM_ID_MAX];
static CRITICAL_SECTION csStrParam[PARAM_ID_STR_MAX];
static CRITICAL_SECTION csDoubleParam[PARAM_ID_DOUBLE_MAX];

/************************************/
/* 関数プロトタイプ					*/
/************************************/
CSharedData::CSharedData() {}
CSharedData::~CSharedData(){}

void CSharedData::InitSharedData(void) {
	for (UINT ii = 0; ii < IMAGE_ID_CAM_MAX; ii++) {
		InitializeCriticalSection(&csImage[ii]);
	}

	for (UINT ii = 0; ii < IMAGE_ID_PROC_MAX; ii++) {
		InitializeCriticalSection(&csProcImage[ii]);
	}

	for (UINT ii = 0; ii < BEVEL_ID_MAX; ii++) {
		InitializeCriticalSection(&csBevel[ii]);
	}

	for (UINT ii = 0; ii < PARAM_ID_MAX; ii++) {
		InitializeCriticalSection(&csParam[ii]);
	}

	for (UINT ii = 0; ii < PARAM_ID_STR_MAX; ii++) {
		InitializeCriticalSection(&csStrParam[ii]);
	}

	for (UINT ii = 0; ii < PARAM_ID_DOUBLE_MAX; ii++) {
		InitializeCriticalSection(&csDoubleParam[ii]);
	}
}

///# 関数: 画像データ設定処理 ***************
INT CSharedData::SetImage(UINT8 id, Mat image) {
	if (id >= IMAGE_ID_CAM_MAX) {
		return RESULT_NG_INVALID;
	}

	EnterCriticalSection(&csImage[id]);
	image.copyTo(m_stImage[id].image);
	m_stImage[id].update = TRUE;
	LeaveCriticalSection(&csImage[id]);

	return RESULT_OK;
}

///# 関数: 画像データ取得処理 ***************
INT CSharedData::GetImage(UINT8 id, Mat* image) {
	if (id >= IMAGE_ID_CAM_MAX) {
		return RESULT_NG_INVALID;
	}
	if (image == NULL) {
		return RESULT_NG_INVALID;
	}
	/* 更新有無の確認 */
	if (m_stImage[id].update == FALSE) {
		return RESULT_NG_SEQUENCE;
	}
	
	EnterCriticalSection(&csImage[id]);
	m_stImage[id].image.copyTo(*image);
	m_stImage[id].update = FALSE;
	LeaveCriticalSection(&csImage[id]);

	return RESULT_OK;
}

///# 関数: 処理画像データ設定処理 ***************
INT CSharedData::SetProcImage(UINT8 id, STProcData data) {
	if (id >= IMAGE_ID_PROC_MAX) {
		return RESULT_NG_INVALID;
	}

	EnterCriticalSection(&csProcImage[id]);
	data.image.copyTo(m_stProcImage[id].data.image);
	m_stProcImage[id].data.posx = data.posx;
	m_stProcImage[id].data.posy = data.posy;
	m_stProcImage[id].update = TRUE;
	LeaveCriticalSection(&csProcImage[id]);

	return RESULT_OK;
}

///# 関数: 処理画像データ取得処理 ***************
INT CSharedData::GetProcImage(UINT8 id, STProcData* data) {
	if (id >= IMAGE_ID_PROC_MAX) {
		return RESULT_NG_INVALID;
	}
	if (data == NULL) {
		return RESULT_NG_INVALID;
	}
	/* 更新有無の確認 */
	if (m_stProcImage[id].update == FALSE) {
		return RESULT_NG_SEQUENCE;
	}

	EnterCriticalSection(&csProcImage[id]);
	m_stProcImage[id].data.image.copyTo(data->image);
	data->posx = m_stProcImage[id].data.posx;
	data->posy = m_stProcImage[id].data.posy;
	m_stProcImage[id].update = FALSE;
	LeaveCriticalSection(&csProcImage[id]);

	return RESULT_OK;
}

///# 関数: 傾斜計データ設定処理 ***************
INT CSharedData::SetBevelData(UINT8 id, DOUBLE data) {
	if (id >= BEVEL_ID_MAX) {
		return RESULT_NG_INVALID;
	}

	EnterCriticalSection(&csBevel[id]);
	m_stBevelData[id].data = data;
	LeaveCriticalSection(&csBevel[id]);

	return RESULT_OK;
}

///# 関数: 傾斜計データ取得処理 ***************
INT CSharedData::GetBevelData(UINT8 id, DOUBLE* data) {
	if (id >= BEVEL_ID_MAX) {
		return RESULT_NG_INVALID;
	}
	if (data == NULL) {
		return RESULT_NG_INVALID;
	}

	EnterCriticalSection(&csBevel[id]);
	*data = m_stBevelData[id].data;
	LeaveCriticalSection(&csBevel[id]);

	return RESULT_OK;
}

///# 関数: データ設定処理 ***************
INT CSharedData::SetParam(UINT8 id, UINT32 data) {
	if (id >= PARAM_ID_MAX) {
		return RESULT_NG_INVALID;
	}

	EnterCriticalSection(&csParam[id]);
	m_u32Param[id] = data;
	LeaveCriticalSection(&csParam[id]);

	return RESULT_OK;
}

///# 関数: データ取得処理 ***************
INT CSharedData::GetParam(UINT8 id, UINT32* data) {
	if (id >= PARAM_ID_MAX) {
		return RESULT_NG_INVALID;
	}
	if (data == NULL) {
		return RESULT_NG_INVALID;
	}

	EnterCriticalSection(&csParam[id]);
	*data = m_u32Param[id];
	LeaveCriticalSection(&csParam[id]);

	return RESULT_OK;
}

///# 関数: データ設定処理 ***************
INT CSharedData::SetParam(UINT8 id, string str) {
	if (id >= PARAM_ID_MAX) {
		return RESULT_NG_INVALID;
	}

	EnterCriticalSection(&csStrParam[id]);
	m_strParam[id] = str;
	LeaveCriticalSection(&csStrParam[id]);

	return RESULT_OK;
}

///# 関数: データ取得処理 ***************
INT CSharedData::GetParam(UINT8 id, string* str) {
	if (id >= PARAM_ID_MAX) {
		return RESULT_NG_INVALID;
	}
	if (str == NULL) {
		return RESULT_NG_INVALID;
	}

	EnterCriticalSection(&csStrParam[id]);
	*str = m_strParam[id];
	LeaveCriticalSection(&csStrParam[id]);

	return RESULT_OK;
}

///# 関数: データ設定処理 ***************
INT CSharedData::SetParam(UINT8 id, DOUBLE data) {
	if (id >= PARAM_ID_MAX) {
		return RESULT_NG_INVALID;
	}

	EnterCriticalSection(&csDoubleParam[id]);
	m_dParam[id] = data;
	LeaveCriticalSection(&csDoubleParam[id]);

	return RESULT_OK;
}

///# 関数: データ取得処理 ***************
INT CSharedData::GetParam(UINT8 id, DOUBLE* data) {
	if (id >= PARAM_ID_MAX) {
		return RESULT_NG_INVALID;
	}
	if (data == NULL) {
		return RESULT_NG_INVALID;
	}

	EnterCriticalSection(&csDoubleParam[id]);
	*data = m_dParam[id];
	LeaveCriticalSection(&csDoubleParam[id]);

	return RESULT_OK;
}