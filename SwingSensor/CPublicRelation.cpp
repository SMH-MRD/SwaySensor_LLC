#include "stdafx.h"
#include "CPublicRelation.h"
#include <Windows.h>
#include <Commdlg.h>

/************************************/
/* �萔��`							*/
/************************************/
#define DISP_PIC_WIDTH	320.0			// �\���摜�T�C�Y����
#define DISP_PIC_HEIGHT 240.0			// �\���摜�T�C�Y����

/************************************/
/* �ϐ���`							*/
/************************************/
static HINSTANCE m_hInst;
static HWND m_hDlg;
static HWND m_hParamDlg;
static CSharedData* m_cSharedData;
static Mat m_saveImage;

/************************************/
/* �֐��v���g�^�C�v					*/
/************************************/
void DispSetDialog(void);
LRESULT CALLBACK DispWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ParamWndProc(HWND, UINT, WPARAM, LPARAM);

CPublicRelation::CPublicRelation(HINSTANCE hInst)
{
	m_hInst = hInst;
	m_cSharedData = new CSharedData();

	m_hDlg = CreateDialog(m_hInst, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, (DLGPROC)DispWndProc);
	ShowWindow(m_hDlg, SW_SHOW);

	TCHAR msg[10];
	_stprintf_s(msg, TEXT("-"));
	SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_GRA_POSX), msg);
	_stprintf_s(msg, TEXT("-"));
	SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_GRA_POSY), msg);
}


CPublicRelation::~CPublicRelation()
{
}

void CPublicRelation::routine_work(void* param) {
	DOUBLE port1Ma, port2Ma, port1Angle, port2Angle;
	TCHAR msg[10];

	/* �X�Όv�f�[�^�X�V */
	if (m_cSharedData->GetBevelData(BEVEL_ID_PORT_1_ANALOG, &port1Ma) == RESULT_OK) {
		if (!isnan(port1Ma)) {
			_stprintf_s(msg, TEXT("%.3f"), port1Ma);
			SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_PORT1_MA), msg);
		}
		else {
			_stprintf_s(msg, TEXT("-"));
			SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_PORT1_MA), msg);
		}
	}
	if (m_cSharedData->GetBevelData(BEVEL_ID_PORT_2_ANALOG, &port2Ma) == RESULT_OK) {
		if (!isnan(port2Ma)) {
			_stprintf_s(msg, TEXT("%.3f"), port2Ma);
			SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_PORT2_MA), msg);
		}
		else {
			_stprintf_s(msg, TEXT("-"));
			SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_PORT2_MA), msg);
		}
	}
	if (m_cSharedData->GetBevelData(BEVEL_ID_PORT_1_ANGLE, &port1Angle) == RESULT_OK) {
		if (!isnan(port1Angle)) {
			_stprintf_s(msg, TEXT("%.3f"), port1Angle);
			SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_PORT1_ANGLE), msg);
		}
		else {
			_stprintf_s(msg, TEXT("-"));
			SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_PORT1_ANGLE), msg);
		}
	}
	if (m_cSharedData->GetBevelData(BEVEL_ID_PORT_2_ANGLE, &port2Angle) == RESULT_OK) {
		if (!isnan(port2Angle)) {
			_stprintf_s(msg, TEXT("%.3f"), port2Angle);
			SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_PORT2_ANGLE), msg);
		}
		else {
			_stprintf_s(msg, TEXT("-"));
			SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_PORT2_ANGLE), msg);
		}
	}

	/* �t���[�����[�g�X�V */
	UINT32 frameRate;
	if (m_cSharedData->GetParam(PARAM_ID_CAM_READ_FRAMERATE, &frameRate) == RESULT_OK) {
		_stprintf_s(msg, TEXT("%d"), frameRate);
		SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_CAM_FRAMERATE), msg);
	}

	/* �摜�X�V */
	HBITMAP bmp;    //�摜(bitmap�t�@�C��)
	STProcData stProcData;
	Mat dispImage;   //�摜�ǂݍ���

	if (m_cSharedData->GetProcImage(IMAGE_ID_PROC_A, &stProcData) != RESULT_OK) {
		if (m_cSharedData->GetProcImage(IMAGE_ID_PROC_B, &stProcData) != RESULT_OK) {
			/* �����ȊO�̂��߁A�I�� */
			return;
		}
	}
	/* �ۑ��p�ɍŐV�摜��ێ����Ă��� */
	stProcData.image.copyTo(m_saveImage);

	resize(stProcData.image, dispImage, cv::Size(), DISP_PIC_WIDTH / stProcData.image.cols, DISP_PIC_HEIGHT / stProcData.image.rows);

	char* ColorBuf = (char*)calloc(dispImage.cols * dispImage.rows * 4, sizeof(RGBQUAD));

	for (int y = 0; y < dispImage.rows; y++) {
		for (int x = 0; x < dispImage.cols; x++) {
			// Blue
			ColorBuf[y * dispImage.cols * 4 + x * 4 + 0] = dispImage.data[y * dispImage.step + x * 3 + 0];
			// Green
			ColorBuf[y * dispImage.cols * 4 + x * 4 + 1] = dispImage.data[y * dispImage.step + x * 3 + 1];
			// Red
			ColorBuf[y * dispImage.cols * 4 + x * 4 + 2] = dispImage.data[y * dispImage.step + x * 3 + 2];
			//Reserved
			ColorBuf[y * dispImage.cols * 4 + x * 4 + 3] = 0;
		}
	}

	bmp = CreateBitmap(dispImage.cols, dispImage.rows, 1, 32, ColorBuf);
	free(ColorBuf);

	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_PIC), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp);
	DeleteObject(bmp);

	if (!isnan(stProcData.posx)) {
		_stprintf_s(msg, TEXT("%.3f"), stProcData.posx);
		SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_GRA_POSX), msg);
	}
	else {
		_stprintf_s(msg, TEXT("-"));
		SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_GRA_POSX), msg);
	}
	if (!isnan(stProcData.posy)) {
		_stprintf_s(msg, TEXT("%.3f"), stProcData.posy);
		SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_GRA_POSY), msg);
	}
	else {
		_stprintf_s(msg, TEXT("-"));
		SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_GRA_POSY), msg);
	}

	Mat maskImage;
	if (m_cSharedData->GetImage(IMAGE_ID_MASK_A, &maskImage) != RESULT_OK) {
		if (m_cSharedData->GetImage(IMAGE_ID_MASK_B, &maskImage) != RESULT_OK) {
			/* �����ȊO�̂��߁A�I�� */
			return;
		}
	}
	resize(maskImage, dispImage, cv::Size(), DISP_PIC_WIDTH / maskImage.cols, DISP_PIC_HEIGHT / maskImage.rows);

	ColorBuf = (char*)calloc(dispImage.cols * dispImage.rows * 4, sizeof(RGBQUAD));

	for (int y = 0; y < dispImage.rows; y++) {
		for (int x = 0; x < dispImage.cols; x++) {
			// Blue
			ColorBuf[y * dispImage.cols * 4 + x * 4 + 0] = dispImage.data[y * dispImage.step + x];
			// Green
			ColorBuf[y * dispImage.cols * 4 + x * 4 + 1] = dispImage.data[y * dispImage.step + x];
			// Red
			ColorBuf[y * dispImage.cols * 4 + x * 4 + 2] = dispImage.data[y * dispImage.step + x];
			//Reserved
			ColorBuf[y * dispImage.cols * 4 + x * 4 + 3] = 0;
		}
	}

	bmp = CreateBitmap(dispImage.cols, dispImage.rows, 1, 32, ColorBuf);
	free(ColorBuf);

	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_PIC_MASK), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp);
	DeleteObject(bmp);

	DOUBLE procTime;
	m_cSharedData->GetParam(PARAM_ID_DOUBLE_PROC_TIME, &procTime);
	if (!isnan(procTime)) {
		_stprintf_s(msg, TEXT("%.3f"), procTime);
		SetWindowText(GetDlgItem(m_hDlg, IDC_EDIT_PROC_TIME), msg);
	}
}

///# �֐�: ���C���_�C�A���O���� ***************
LRESULT CALLBACK DispWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	INT wmId = LOWORD(wp);

	switch (msg) {
	case WM_INITDIALOG:

		break;
	case WM_COMMAND:
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case IDC_BUTTON_CAM_START:
			m_cSharedData->SetParam(PARAM_ID_CAM_PROC, (UINT32)TRUE);
			EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_PIC_PROC), FALSE);
			break;
		case IDC_BUTTON_CAM_STOP:
			m_cSharedData->SetParam(PARAM_ID_CAM_PROC, (UINT32)FALSE);
			EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_PIC_PROC), TRUE);
			break;
		case IDC_BUTTON_SAVE:
#if 1
			if (m_saveImage.empty() != TRUE) {
				imwrite("procImage.bmp", m_saveImage);
			}
			else {
				MessageBox(m_hDlg, L"�ۑ�����摜������܂���B", L"�G���[", MB_OK);
			}
#endif
			break;
		case IDC_BUTTON_OPEN_SET:
			DispSetDialog();
			break;
		case IDC_BUTTON_PIC_PROC: {
			static OPENFILENAME ofn;
			static TCHAR strFile[MAX_PATH], strCustom[256] = TEXT("Before files\0*.*\0\0");

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = m_hDlg;
			ofn.lpstrFilter = TEXT("Image files {*.jpg;*.bmp;*.png}\0*.jpg;*.bmp;*.png\0")
				TEXT("All files {*.*}\0*.*\0\0");
			ofn.lpstrCustomFilter = strCustom;
			ofn.nMaxCustFilter = 256;
			ofn.nFilterIndex = 0;
			ofn.lpstrFile = strFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn) == TRUE) {
				char* cstr = (char*)malloc(sizeof(strFile));
				if (cstr != NULL) {
					size_t size;
					wcstombs_s(&size, cstr, sizeof(strFile), strFile, sizeof(strFile));
					string str = cstr;
					m_cSharedData->SetParam(PARAM_ID_STR_PROC_FILENAME, str);
					free(cstr);
					m_cSharedData->SetParam(PARAM_ID_PIC_PROC_FLAG, (UINT32)TRUE);
				}
			}

			break;
		}
		case IDCANCEL:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_PAINT:

		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}

	return FALSE;
}

///# �֐�: �p�����[�^�_�C�A���O�\������ ***************
void DispSetDialog(void)
{
	m_hParamDlg = CreateDialog(m_hInst, MAKEINTRESOURCE(IDD_DIALOG2), nullptr, (DLGPROC)ParamWndProc);
	ShowWindow(m_hParamDlg, SW_SHOW);

	/* �e�\���p�����[�^������ */
	TCHAR msg[10];
	UINT value = 0;

	m_cSharedData->GetParam(PARAM_ID_CAM_EXPOSURE_TIME, &value);
	_stprintf_s(msg, TEXT("%d"), value);
	SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_EXPOSURE_TIME), msg);
	m_cSharedData->GetParam(PARAM_ID_CAM_WIDTH, &value);
	_stprintf_s(msg, TEXT("%d"), value);
	SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_CAP_WIDTH), msg);
	m_cSharedData->GetParam(PARAM_ID_CAM_HEIGHT, &value);
	_stprintf_s(msg, TEXT("%d"), value);
	SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_CAP_HEIGHT), msg);
	m_cSharedData->GetParam(PARAM_ID_CAM_FRAMERATE, &value);
	_stprintf_s(msg, TEXT("%d"), value);
	SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_FRAMERATE), msg);

	m_cSharedData->GetParam(PARAM_ID_PIC_HUE1_EN, &value);
	if (value == TRUE) {
		SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE1_EN), BM_SETCHECK, BST_CHECKED, 0);
	}
	else {
		SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE1_EN), BM_SETCHECK, BST_UNCHECKED, 0);
	}
	m_cSharedData->GetParam(PARAM_ID_PIC_HUE1_MIN, &value);
	_stprintf_s(msg, TEXT("%d"), value);
	SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE1_MIN), msg);
	m_cSharedData->GetParam(PARAM_ID_PIC_HUE1_MAX, &value);
	_stprintf_s(msg, TEXT("%d"), value);
	SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE1_MAX), msg);

	m_cSharedData->GetParam(PARAM_ID_PIC_HUE2_EN, &value);
	if (value == TRUE) {
		SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE2_EN), BM_SETCHECK, BST_CHECKED, 0);
	}
	else {
		SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE2_EN), BM_SETCHECK, BST_UNCHECKED, 0);
	}
	m_cSharedData->GetParam(PARAM_ID_PIC_HUE2_MIN, &value);
	_stprintf_s(msg, TEXT("%d"), value);
	SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE2_MIN), msg);
	m_cSharedData->GetParam(PARAM_ID_PIC_HUE2_MAX, &value);
	_stprintf_s(msg, TEXT("%d"), value);
	SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE2_MAX), msg);

	m_cSharedData->GetParam(PARAM_ID_PIC_HUE3_EN, &value);
	if (value == TRUE) {
		SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE3_EN), BM_SETCHECK, BST_CHECKED, 0);
	}
	else {
		SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE3_EN), BM_SETCHECK, BST_UNCHECKED, 0);
	}
	m_cSharedData->GetParam(PARAM_ID_PIC_HUE3_MIN, &value);
	_stprintf_s(msg, TEXT("%d"), value);
	SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE3_MIN), msg);
	m_cSharedData->GetParam(PARAM_ID_PIC_HUE3_MAX, &value);
	_stprintf_s(msg, TEXT("%d"), value);
	SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE3_MAX), msg);

	m_cSharedData->GetParam(PARAM_ID_PIC_COG_ALGO, &value);
	if (value == COG_ALGO_KIND_ALL) {
		SendMessage(GetDlgItem(m_hParamDlg, IDC_RADIO_COG_ALL), BM_SETCHECK, BST_CHECKED, 0);
	}
	else {
		SendMessage(GetDlgItem(m_hParamDlg, IDC_RADIO_COG_LEN), BM_SETCHECK, BST_CHECKED, 0);
	}
}

///# �֐�: �p�����[�^�_�C�A���O���� ***************
LRESULT CALLBACK ParamWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	INT lmId = LOWORD(wp);
	INT hmId = HIWORD(wp);
	TCHAR buf[20];
	UINT32 value;
	TCHAR valueStr[10];
	wstring str;

	switch (msg) {
	case WM_INITDIALOG:

		break;
	case WM_COMMAND:
		// �I�����ꂽ���j���[�̉��:
		switch (lmId)
		{
		case IDC_BUTTON_SET_CAM_PARAM:
			/* ���͒l�`�F�b�N���{ */
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_EXPOSURE_TIME), buf, 20);
			str = buf;
			if ((!all_of(str.cbegin(), str.cend(), isdigit)) || str.empty()) {
				MessageBox(m_hParamDlg, L"�I������(usec)�ɂ͐��l�݂̂���͂��Ă��������B", L"�G���[", MB_OK);
				return FALSE;
			}
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_CAP_WIDTH), buf, 20);
			str = buf;
			if ((!all_of(str.cbegin(), str.cend(), isdigit)) || str.empty()) {
				MessageBox(m_hParamDlg, L"�L���v�`���T�C�Y(����)�ɂ͐��l�݂̂���͂��Ă��������B", L"�G���[", MB_OK);
				return FALSE;
			}
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_CAP_HEIGHT), buf, 20);
			str = buf;
			if ((!all_of(str.cbegin(), str.cend(), isdigit)) || str.empty()) {
				MessageBox(m_hParamDlg, L"�L���v�`���T�C�Y(����)�ɂ͐��l�݂̂���͂��Ă��������B", L"�G���[", MB_OK);
				return FALSE;
			}
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_FRAMERATE), buf, 20);
			str = buf;
			if ((!all_of(str.cbegin(), str.cend(), isdigit)) || str.empty()) {
				MessageBox(m_hParamDlg, L"�t���[�����[�g�ɂ͐��l�݂̂���͂��Ă��������B", L"�G���[", MB_OK);
				return FALSE;
			}

			/* �ݒ�l�����L�I�u�W�F�N�g�ɐݒ� */
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_EXPOSURE_TIME), buf, 20);
			value = _wtoi(buf);
			m_cSharedData->SetParam(PARAM_ID_CAM_EXPOSURE_TIME, value);

			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_CAP_WIDTH), buf, 20);
			value = _wtoi(buf);
			if ((value % 32) != 0) {
				value = value - (value % 32);
			}
			if (value == 0) {
				value = 32;
			}
			_stprintf_s(valueStr, TEXT("%d"), value);
			SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_CAP_WIDTH), valueStr);
			m_cSharedData->SetParam(PARAM_ID_CAM_WIDTH, value);

			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_CAP_HEIGHT), buf, 20);
			value = _wtoi(buf);
			if ((value % 2) != 0) {
				value = value - (value % 2);
			}
			if (value == 0) {
				value = 2;
			}
			_stprintf_s(valueStr, TEXT("%d"), value);
			SetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_CAP_HEIGHT), valueStr);
			m_cSharedData->SetParam(PARAM_ID_CAM_HEIGHT, value);
			
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_FRAMERATE), buf, 20);
			value = _wtoi(buf);
			m_cSharedData->SetParam(PARAM_ID_CAM_FRAMERATE, value);
			break;
		case IDC_CHECK_HUE1_EN:
			if (BST_CHECKED == SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE1_EN), BM_GETCHECK, 0, 0)) {
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE1_MIN), EM_SETREADONLY, 0, 0);
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE1_MAX), EM_SETREADONLY, 0, 0);
			}
			else {
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE1_MIN), EM_SETREADONLY, 1, 0);
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE1_MAX), EM_SETREADONLY, 1, 0);
			}
			break;
		case IDC_CHECK_HUE2_EN:
			if (BST_CHECKED == SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE2_EN), BM_GETCHECK, 0, 0)) {
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE2_MIN), EM_SETREADONLY, 0, 0);
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE2_MAX), EM_SETREADONLY, 0, 0);
			}
			else {
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE2_MIN), EM_SETREADONLY, 1, 0);
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE2_MAX), EM_SETREADONLY, 1, 0);
			}
			break;
		case IDC_CHECK_HUE3_EN:
			if (BST_CHECKED == SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE3_EN), BM_GETCHECK, 0, 0)) {
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE3_MIN), EM_SETREADONLY, 0, 0);
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE3_MAX), EM_SETREADONLY, 0, 0);
			}
			else {
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE3_MIN), EM_SETREADONLY, 1, 0);
				SendMessage(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE3_MAX), EM_SETREADONLY, 1, 0);
			}
			break;
		case IDC_BUTTON_SET_PICPROC_PARAM:
			/* ���͒l�`�F�b�N���{ */
			value = SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE1_EN), BM_GETCHECK, 0, 0);
			if (value == TRUE) {
				GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE1_MIN), buf, 20);
				str = buf;
				if ((!all_of(str.cbegin(), str.cend(), isdigit)) || str.empty()) {
					MessageBox(m_hParamDlg, L"�F���}�X�N1�ŏ��ɂ͐��l�݂̂���͂��Ă��������B", L"�G���[", MB_OK);
					return FALSE;
				}
				GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE1_MAX), buf, 20);
				str = buf;
				if ((!all_of(str.cbegin(), str.cend(), isdigit)) || str.empty()) {
					MessageBox(m_hParamDlg, L"�F���}�X�N1�ő�ɂ͐��l�݂̂���͂��Ă��������B", L"�G���[", MB_OK);
					return FALSE;
				}
			}
			value = SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE2_EN), BM_GETCHECK, 0, 0);
			if (value == TRUE) {
				GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE2_MIN), buf, 20);
				str = buf;
				if ((!all_of(str.cbegin(), str.cend(), isdigit)) || str.empty()) {
					MessageBox(m_hParamDlg, L"�F���}�X�N2�ŏ��ɂ͐��l�݂̂���͂��Ă��������B", L"�G���[", MB_OK);
					return FALSE;
				}
				GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE2_MAX), buf, 20);
				str = buf;
				if ((!all_of(str.cbegin(), str.cend(), isdigit)) || str.empty()) {
					MessageBox(m_hParamDlg, L"�F���}�X�N2�ő�ɂ͐��l�݂̂���͂��Ă��������B", L"�G���[", MB_OK);
					return FALSE;
				}
			}
			value = SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE3_EN), BM_GETCHECK, 0, 0);
			if (value == TRUE) {
				GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE3_MIN), buf, 20);
				str = buf;
				if ((!all_of(str.cbegin(), str.cend(), isdigit)) || str.empty()) {
					MessageBox(m_hParamDlg, L"�F���}�X�N3�ŏ��ɂ͐��l�݂̂���͂��Ă��������B", L"�G���[", MB_OK);
					return FALSE;
				}
				GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE3_MAX), buf, 20);
				str = buf;
				if ((!all_of(str.cbegin(), str.cend(), isdigit)) || str.empty()) {
					MessageBox(m_hParamDlg, L"�F���}�X�N3�ő�ɂ͐��l�݂̂���͂��Ă��������B", L"�G���[", MB_OK);
					return FALSE;
				}
			}

			/* �ݒ�l�����L�I�u�W�F�N�g�ɐݒ� */
			value = SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE1_EN), BM_GETCHECK, 0, 0);
			m_cSharedData->SetParam(PARAM_ID_PIC_HUE1_EN, value);
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE1_MIN), buf, 20);
			value = _wtoi(buf);
			m_cSharedData->SetParam(PARAM_ID_PIC_HUE1_MIN, value);
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE1_MAX), buf, 20);
			value = _wtoi(buf);
			m_cSharedData->SetParam(PARAM_ID_PIC_HUE1_MAX, value);

			value = SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE2_EN), BM_GETCHECK, 0, 0);
			m_cSharedData->SetParam(PARAM_ID_PIC_HUE2_EN, value);
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE2_MIN), buf, 20);
			value = _wtoi(buf);
			m_cSharedData->SetParam(PARAM_ID_PIC_HUE2_MIN, value);
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE2_MAX), buf, 20);
			value = _wtoi(buf);
			m_cSharedData->SetParam(PARAM_ID_PIC_HUE2_MAX, value);

			value = SendMessage(GetDlgItem(m_hParamDlg, IDC_CHECK_HUE3_EN), BM_GETCHECK, 0, 0);
			m_cSharedData->SetParam(PARAM_ID_PIC_HUE3_EN, value);
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE3_MIN), buf, 20);
			value = _wtoi(buf);
			m_cSharedData->SetParam(PARAM_ID_PIC_HUE3_MIN, value);
			GetWindowText(GetDlgItem(m_hParamDlg, IDC_EDIT_HUE3_MAX), buf, 20);
			value = _wtoi(buf);
			m_cSharedData->SetParam(PARAM_ID_PIC_HUE3_MAX, value);

			value = SendMessage(GetDlgItem(m_hParamDlg, IDC_RADIO_COG_ALL), BM_GETCHECK, 0, 0);
			if (value == TRUE) {
				m_cSharedData->SetParam(PARAM_ID_PIC_COG_ALGO, (UINT32)COG_ALGO_KIND_ALL);
			}
			else {
				m_cSharedData->SetParam(PARAM_ID_PIC_COG_ALGO, (UINT32)COG_ALGO_KIND_LEN);
			}
			break;
		}
		break;
	case WM_PAINT:

		break;
	case WM_CLOSE:
		EndDialog(m_hParamDlg, WM_CLOSE);
		break;
	}
	return FALSE;
}