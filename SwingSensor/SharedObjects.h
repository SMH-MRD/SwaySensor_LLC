#pragma once

#include "stdafx.h"
#include "CommonFormat.h"

#define SYSTEM_TICK_ms		25		// �X���b�h����

enum {
	IMAGE_ID_CAM_A = 0,		// �J�����摜A
	IMAGE_ID_CAM_B,			// �J�����摜B
	IMAGE_ID_MASK_A,		// �}�X�N�摜A
	IMAGE_ID_MASK_B,		// �}�X�N�摜B
	IMAGE_ID_CAM_MAX
};

enum {
	IMAGE_ID_PROC_A = 0,	// ���H��摜A
	IMAGE_ID_PROC_B,		// ���H��摜B
	IMAGE_ID_PROC_MAX
};

enum {
	BEVEL_ID_PORT_1_ANALOG = 0,		// RIO PORT1���͒l(���l)
	BEVEL_ID_PORT_2_ANALOG,			// RIO PORT2���͒l(���l)
	BEVEL_ID_PORT_1_MA,				// RIO PORT1���͒l(mA)
	BEVEL_ID_PORT_2_MA,				// RIO PORT2���͒l(mA)
	BEVEL_ID_PORT_1_ANGLE,			// RIO PORT1���͒l(�p�x)
	BEVEL_ID_PORT_2_ANGLE,			// RIO PORT2���͒l(�p�x)
	BEVEL_ID_MAX
};

enum {
	PARAM_ID_CAM_PROC = 0,				// �J��������
	PARAM_ID_CAM_EXPOSURE_TIME,			// �J�����ݒ�(�I������)
	PARAM_ID_CAM_FRAMERATE,			// �J�����ݒ�(�t���[�����[�g)
	PARAM_ID_CAM_WIDTH,					// �J�����ݒ�(�L���v�`���T�C�Y����)
	PARAM_ID_CAM_HEIGHT,				// �J�����ݒ�(�L���v�`���T�C�Y����)
	PARAM_ID_CAM_READ_FRAMERATE,		// �J�����Ǐo��(�t���[�����[�g)
	PARAM_ID_PIC_PROC_FLAG,				// �摜��͎��s�t���O
	PARAM_ID_PIC_HUE1_EN,				// �摜�����ݒ�(�F���}�X�N1�L������)
	PARAM_ID_PIC_HUE1_MIN,				// �摜�����ݒ�(�F���}�X�N1�Œ�l)
	PARAM_ID_PIC_HUE1_MAX,				// �摜�����ݒ�(�F���}�X�N1�ő�l)
	PARAM_ID_PIC_HUE2_EN,				// �摜�����ݒ�(�F���}�X�N2�L������)
	PARAM_ID_PIC_HUE2_MIN,				// �摜�����ݒ�(�F���}�X�N2�Œ�l)
	PARAM_ID_PIC_HUE2_MAX,				// �摜�����ݒ�(�F���}�X�N2�ő�l)
	PARAM_ID_PIC_HUE3_EN,				// �摜�����ݒ�(�F���}�X�N3�L������)
	PARAM_ID_PIC_HUE3_MIN,				// �摜�����ݒ�(�F���}�X�N3�Œ�l)
	PARAM_ID_PIC_HUE3_MAX,				// �摜�����ݒ�(�F���}�X�N3�ő�l)
	PARAM_ID_PIC_COG_ALGO,				// �摜�����ݒ�(�d�S�ʒu�Z�o�A���S���Y��)
	PARAM_ID_RIO_TCPPORT,				// RIO TCP�|�[�g�ԍ�
	PARAM_ID_RIO_SLAVEADDR,				// RIO�X���[�u�A�h���X
	PARAM_ID_RIO_TIMEOUT,				// RIO�^�C���A�E�g
	PARAM_ID_RIO_XPORT,					// RIO�X�ΌvX���f�[�^�ڑ��|�[�g�ԍ�
	PARAM_ID_RIO_YPORT,					// RIO�X�ΌvY���f�[�^�ڑ��|�[�g�ԍ�
	PARAM_ID_MAX
};

enum {
	PARAM_ID_STR_PROC_FILENAME = 0,		// ��͑Ώۉ摜�t�@�C����
	PARAM_ID_STR_RIO_IPADDR,			// RIO IP�A�h���X
	PARAM_ID_STR_MAX
};

enum {
	COG_ALGO_KIND_ALL = 0,				// �d�S�ʒu�Z�o�A���S���Y��(�S�֊s�_)
	COG_ALGO_KIND_LEN					// �d�S�ʒu�Z�o�A���S���Y��(�ő�֊s��)
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

