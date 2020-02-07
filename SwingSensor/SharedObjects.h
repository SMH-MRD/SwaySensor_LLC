#pragma once

#include "stdafx.h"
#include "CommonFormat.h"

#define SYSTEM_TICK_ms		25		// スレッド周期

enum {
	IMAGE_ID_CAM_A = 0,		// カメラ画像A
	IMAGE_ID_CAM_B,			// カメラ画像B
	IMAGE_ID_MASK_A,		// マスク画像A
	IMAGE_ID_MASK_B,		// マスク画像B
	IMAGE_ID_CAM_MAX
};

enum {
	IMAGE_ID_PROC_A = 0,	// 加工後画像A
	IMAGE_ID_PROC_B,		// 加工後画像B
	IMAGE_ID_PROC_MAX
};

enum {
	BEVEL_ID_PORT_1_ANALOG = 0,		// RIO PORT1入力値(生値)
	BEVEL_ID_PORT_2_ANALOG,			// RIO PORT2入力値(生値)
	BEVEL_ID_PORT_1_MA,				// RIO PORT1入力値(mA)
	BEVEL_ID_PORT_2_MA,				// RIO PORT2入力値(mA)
	BEVEL_ID_PORT_1_ANGLE,			// RIO PORT1入力値(角度)
	BEVEL_ID_PORT_2_ANGLE,			// RIO PORT2入力値(角度)
	BEVEL_ID_MAX
};

enum {
	PARAM_ID_CAM_PROC = 0,				// カメラ制御
	PARAM_ID_CAM_EXPOSURE_TIME,			// カメラ設定(露光時間)
	PARAM_ID_CAM_FRAMERATE,			// カメラ設定(フレームレート)
	PARAM_ID_CAM_WIDTH,					// カメラ設定(キャプチャサイズ横幅)
	PARAM_ID_CAM_HEIGHT,				// カメラ設定(キャプチャサイズ高さ)
	PARAM_ID_CAM_READ_FRAMERATE,		// カメラ読出し(フレームレート)
	PARAM_ID_PIC_PROC_FLAG,				// 画像解析実行フラグ
	PARAM_ID_PIC_HUE1_EN,				// 画像処理設定(色相マスク1有効無効)
	PARAM_ID_PIC_HUE1_MIN,				// 画像処理設定(色相マスク1最低値)
	PARAM_ID_PIC_HUE1_MAX,				// 画像処理設定(色相マスク1最大値)
	PARAM_ID_PIC_HUE2_EN,				// 画像処理設定(色相マスク2有効無効)
	PARAM_ID_PIC_HUE2_MIN,				// 画像処理設定(色相マスク2最低値)
	PARAM_ID_PIC_HUE2_MAX,				// 画像処理設定(色相マスク2最大値)
	PARAM_ID_PIC_HUE3_EN,				// 画像処理設定(色相マスク3有効無効)
	PARAM_ID_PIC_HUE3_MIN,				// 画像処理設定(色相マスク3最低値)
	PARAM_ID_PIC_HUE3_MAX,				// 画像処理設定(色相マスク3最大値)
	PARAM_ID_PIC_COG_ALGO,				// 画像処理設定(重心位置算出アルゴリズム)
	PARAM_ID_RIO_TCPPORT,				// RIO TCPポート番号
	PARAM_ID_RIO_SLAVEADDR,				// RIOスレーブアドレス
	PARAM_ID_RIO_TIMEOUT,				// RIOタイムアウト
	PARAM_ID_RIO_XPORT,					// RIO傾斜計X軸データ接続ポート番号
	PARAM_ID_RIO_YPORT,					// RIO傾斜計Y軸データ接続ポート番号
	PARAM_ID_MAX
};

enum {
	PARAM_ID_STR_PROC_FILENAME = 0,		// 解析対象画像ファイル名
	PARAM_ID_STR_RIO_IPADDR,			// RIO IPアドレス
	PARAM_ID_STR_MAX
};

enum {
	COG_ALGO_KIND_ALL = 0,				// 重心位置算出アルゴリズム(全輪郭点)
	COG_ALGO_KIND_LEN					// 重心位置算出アルゴリズム(最大輪郭長)
};

typedef struct _stProcData
{
	Mat image;
	double posx;
	double posy;
}STProcData;


class CSharedData
{
public:
	CSharedData();
	~CSharedData();

	void InitSharedData(void);

	INT SetImage(UINT8 id, Mat image);
	INT GetImage(UINT8 id, Mat* image);

	INT SetProcImage(UINT8 id, STProcData data);
	INT GetProcImage(UINT8 id, STProcData* data);

	INT SetBevelData(UINT8 id, DOUBLE data);
	INT GetBevelData(UINT8 id, DOUBLE* data);

	INT SetParam(UINT8 id, UINT32 data);
	INT GetParam(UINT8 id, UINT32* data);
	INT SetParam(UINT8 id, string str);
	INT GetParam(UINT8 id, string* str);
};

