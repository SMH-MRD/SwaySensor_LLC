#include "stdafx.h"
#include "CAnalyst.h"
#include "SharedObjects.h"

/************************************/
/* �萔��`							*/
/************************************/
#define S_MAX 255
#define S_MIN 50
#define V_MAX 255
#define V_MIN 50

/************************************/
/* �ϐ���`							*/
/************************************/
static CSharedData* m_cSharedData;
static BOOL m_bSaveImageArea;

/************************************/
/* �֐��v���g�^�C�v					*/
/************************************/
static void CalcCenterOfGravity1(InputOutputArray image, vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY);
static void CalcCenterOfGravity2(InputOutputArray image, vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY);

CAnalyst::CAnalyst()
{
	m_cSharedData = new CSharedData();
}


CAnalyst::~CAnalyst()
{
}

void CAnalyst::init_task(void* pobj) {
}

void CAnalyst::routine_work(void* param) {
	ImageProc();
	BevelProc();
}

///# �摜���� ***************
void CAnalyst::ImageProc(void) {
	// �摜�ϊ�����
	Mat image;
	UINT32 width = 0, height = 0;

	// �������Ԍv��(�J�n���Ԏ擾)
	LARGE_INTEGER    frequency;
	QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER    start;
	QueryPerformanceCounter(&start);

	if (m_cSharedData->GetImage(IMAGE_ID_CAM_A, &image) != RESULT_OK) {
		if (m_cSharedData->GetImage(IMAGE_ID_CAM_B, &image) != RESULT_OK) {
			/* �����ȊO�̂��߁A�I�� */
			return;
		}
	}

//	image = imread("test3.png", IMREAD_COLOR);

	Mat hsvImage;
	/* �@�摜�F��BGR��HSV�ɕϊ� */
	cvtColor(image, hsvImage, COLOR_BGR2HSV);

//	namedWindow("hsvImage", WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);//other options:CV_AUTOSIZE,CV_FREERATIO

//	imshow("hsvImage", hsvImage);

	/* �A�F���Ń}�X�N�摜�쐬 */
	UINT32 maskEn, maskMin, maskMax;
	Mat maskImage1, maskImage2, maskImage3;
	m_cSharedData->GetParam(PARAM_ID_PIC_HUE1_EN, &maskEn);
	if (maskEn == TRUE) {
		/* �}�X�N1���� */
		m_cSharedData->GetParam(PARAM_ID_PIC_HUE1_MIN, &maskMin);
		m_cSharedData->GetParam(PARAM_ID_PIC_HUE1_MAX, &maskMax);
		Scalar mask1Min = Scalar(maskMin, S_MIN, V_MIN);
		Scalar mask1Max = Scalar(maskMax, S_MAX, V_MAX);
		inRange(hsvImage, mask1Min, mask1Max, maskImage1);
	}
	else {
		Scalar mask1Min = Scalar(0, S_MIN, V_MIN);
		Scalar mask1Max = Scalar(0, S_MAX, V_MAX);
		inRange(hsvImage, mask1Min, mask1Max, maskImage1);
	}

	m_cSharedData->GetParam(PARAM_ID_PIC_HUE2_EN, &maskEn);
	if (maskEn == TRUE) {
		/* �}�X�N2���� */
		m_cSharedData->GetParam(PARAM_ID_PIC_HUE2_MIN, &maskMin);
		m_cSharedData->GetParam(PARAM_ID_PIC_HUE2_MAX, &maskMax);
		Scalar mask2Min = Scalar(maskMin, S_MIN, V_MIN);
		Scalar mask2Max = Scalar(maskMax, S_MAX, V_MAX);
		inRange(hsvImage, mask2Min, mask2Max, maskImage2);
	}
	else {
		Scalar mask2Min = Scalar(0, S_MIN, V_MIN);
		Scalar mask2Max = Scalar(0, S_MAX, V_MAX);
		inRange(hsvImage, mask2Min, mask2Max, maskImage2);
	}

	m_cSharedData->GetParam(PARAM_ID_PIC_HUE3_EN, &maskEn);
	if (maskEn == TRUE) {
		/* �}�X�N3���� */
		m_cSharedData->GetParam(PARAM_ID_PIC_HUE3_MIN, &maskMin);
		m_cSharedData->GetParam(PARAM_ID_PIC_HUE3_MAX, &maskMax);
		Scalar mask3Min = Scalar(maskMin, S_MIN, V_MIN);
		Scalar mask3Max = Scalar(maskMax, S_MAX, V_MAX);
		inRange(hsvImage, mask3Min, mask3Max, maskImage3);
	}
	else {
		Scalar mask3Min = Scalar(0, S_MIN, V_MIN);
		Scalar mask3Max = Scalar(0, S_MAX, V_MAX);
		inRange(hsvImage, mask3Min, mask3Max, maskImage3);
	}

	/* �F���}�X�N���� */
	Mat maskImage;
	maskImage = maskImage1 + maskImage2 + maskImage3;

	/* �BV�摜�Ƀ}�X�N���|������A2�l�� */
	Mat splitImage[3];
	Mat maskedImg;
	split(hsvImage, splitImage);
	bitwise_and(splitImage[2], splitImage[2], maskedImg, maskImage);

	Mat binarizeImage;
	threshold(maskedImg, binarizeImage, 100, 255, THRESH_BINARY);
	if (m_bSaveImageArea == FALSE) {
		m_cSharedData->SetImage(IMAGE_ID_MASK_A, binarizeImage);
		m_bSaveImageArea = TRUE;
	}
	else {
		m_cSharedData->SetImage(IMAGE_ID_MASK_B, binarizeImage);
		m_bSaveImageArea = FALSE;
	}

//	namedWindow("maskImage", WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);//other options:CV_AUTOSIZE,CV_FREERATIO

//	imshow("maskImage", binarizeImage);

	/* �C�̈撊�o(ROI)�Ń^�[�Q�b�g�F�� */
	/* �֊s���o */
	vector<vector<Point>> contours;
	findContours(binarizeImage, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	/* �D���^�[�Q�b�g�̐M�����]�� */

	/* �E�d�S�ʒu�v�Z */
	double posX = 0;
	double posY = 0;

	UINT32 algo;
	m_cSharedData->GetParam(PARAM_ID_PIC_COG_ALGO, &algo);
	if (algo == COG_ALGO_KIND_ALL) {
		CalcCenterOfGravity1(image, contours, &posX, &posY);
	}
	else {
		CalcCenterOfGravity2(image, contours, &posX, &posY);
	}

//	namedWindow("procImage", WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);//other options:CV_AUTOSIZE,CV_FREERATIO

//	imshow("procImage", image);

//	waitKey(0);

	STProcData stProcData;
	stProcData.image = image;
	stProcData.posx = posX;
	stProcData.posy = posY;
	if (m_bSaveImageArea == FALSE) {
		m_cSharedData->SetProcImage(IMAGE_ID_PROC_A, stProcData);
		m_bSaveImageArea = TRUE;
	}
	else {
		m_cSharedData->SetProcImage(IMAGE_ID_PROC_B, stProcData);
		m_bSaveImageArea = FALSE;
	}

	// �������Ԍv��(�I�����Ԏ擾)
	LARGE_INTEGER    end;
	QueryPerformanceCounter(&end);
	LONGLONG span = end.QuadPart - start.QuadPart;
	LONGLONG usec = (span * 1000000L) / frequency.QuadPart;
}

///# �X�Όv�f�[�^���� ***************
void CAnalyst::BevelProc(void) {
	// �X�Όv�f�[�^����
	DOUBLE port1AnaData, port2AnaData;
	DOUBLE port1AngleData, port2AngleData;

	if (m_cSharedData->GetBevelData(BEVEL_ID_PORT_1_MA, &port1AnaData) == RESULT_OK) {
		if (isnan(port1AnaData)) {
			port1AngleData = NAN;
		}
		else if (port1AnaData < 12.0) {
			// 12.0mA�����Ȃ�Ίp�x��-�����ɓ|��Ă���(4mA��-30�x)
			port1AngleData = ((port1AnaData - 4.0) - 8.0) / 8.0 * 30.0;
		}
		else {
			// 12.0mA�ȏ�Ȃ�Ίp�x��+�����ɓ|��Ă���(20mA��30�x)
			port1AngleData = (port1AnaData - 12.0) / 8.0 * 30.0;
		}
		m_cSharedData->SetBevelData(BEVEL_ID_PORT_1_ANGLE, port1AngleData);
	}

	if (m_cSharedData->GetBevelData(BEVEL_ID_PORT_2_MA, &port2AnaData) == RESULT_OK) {
		if (isnan(port2AnaData)) {
			port2AngleData = NAN;
		}
		else if (port2AnaData < 12.0) {
			// 12.0mA�����Ȃ�Ίp�x��-�����ɓ|��Ă���(4mA��-30�x)
			port2AngleData = ((port2AnaData - 4.0) - 8.0) / 8.0 * 30.0;
		}
		else {
			// 12.0mA�ȏ�Ȃ�Ίp�x��+�����ɓ|��Ă���(20mA��30�x)
			port2AngleData = (port2AnaData - 12.0) / 8.0 * 30.0;
		}
		m_cSharedData->SetBevelData(BEVEL_ID_PORT_2_ANGLE, port2AngleData);
	}
}

///# �֐�: �d�S�ʒu�Z�o�A���S���Y��1 ***************
static void CalcCenterOfGravity1(InputOutputArray image, vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY) {
	double posX = 0;
	double posY = 0;
	UINT32 count = 0;
	/* �֊s�S�_����d�S�ʒu���Z�o */
	for (UINT ii = 0; ii < contours.size(); ii++) {
		UINT tempCount = contours.at(ii).size();
		count += tempCount;
		for (UINT jj = 0; jj < tempCount; jj++) {
			posX += contours.at(ii).at(jj).x;
			posY += contours.at(ii).at(jj).y;
		}
	}

	posX /= count;
	posY /= count;

	circle(image, Point((INT)posX, (INT)posY), 30, Scalar(0, 0, 255), 3, 4);
	*outPosX = posX;
	*outPosY = posY;
}

///# �֐�: �d�S�ʒu�Z�o�A���S���Y��2 ***************
static void CalcCenterOfGravity2(InputOutputArray image, vector<vector<Point>> contours, DOUBLE* outPosX, DOUBLE* outPosY) {
	double max_size = 0;
	double max2_size = 0;
	int max_id = -1;
	int max2_id = -1;

	if (contours.size() == 0) {
		/* �Ώۗ֊s�����݂��Ȃ��ꍇ */
		*outPosX = log(-1.0);
		*outPosY = log(-1.0);
		return;
	}

	/* �֊s��������2�̗֊s�����Ώۂ𒊏o���� */
	for (size_t ii = 0; ii < contours.size(); ii++) {
		if (contours[ii].size() > max_size) {
			max_size = contours[ii].size();
			max_id = ii;
		}
		else if (contours[ii].size() > max2_size) {
			max2_size = contours[ii].size();
			max2_id = ii;
		}
	}
	if ((max_id <= 0) || (max2_id <= 0)) {
		/* �Ώۗ֊s�����݂��Ȃ��ꍇ */
		*outPosX = log(-1.0);
		*outPosY = log(-1.0);
		return;
	}

	Moments mu1 = moments(contours[max_id]);
	Point2f mc1 = Point2f((float)mu1.m10 / (float)mu1.m00, (float)mu1.m01 / (float)mu1.m00);
	Moments mu2 = moments(contours[max2_id]);
	Point2f mc2 = Point2f((float)mu2.m10 / (float)mu2.m00, (float)mu2.m01 / (float)mu2.m00);

	circle(image, Point((INT)((mc1.x + mc2.x) / 2), (INT)((mc1.y + mc2.y) / 2)), 30, Scalar(0, 0, 255), 3, 4);
	*outPosX = ((DOUBLE)mc1.x + mc2.x) / 2.0;
	*outPosY = ((DOUBLE)mc1.y + mc2.y) / 2.0;
}