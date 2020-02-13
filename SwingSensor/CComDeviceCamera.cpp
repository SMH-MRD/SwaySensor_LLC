#include "stdafx.h"
#include "CComDeviceCamera.h"
#include "Helper.h"
#include "SharedObjects.h"

/************************************/
/* �萔��`							*/
/************************************/
#define saveImages 0
#define recordVideo 0

/************************************/
/* �ϐ���`							*/
/************************************/
static const UINT32 c_countOfImagesToGrab = 1;
static BOOL m_bSaveImageArea = FALSE;
static CSharedData* m_cSharedData;
static CBaslerUniversalInstantCamera* camera;
static VideoWriter cvVideoCreator;

/************************************/
/* �֐��v���g�^�C�v					*/
/************************************/
static void CameraCaptureStart(void);
static void ImageProcStart(void);

CComDeviceCamera::CComDeviceCamera() {
	m_cSharedData = new CSharedData();
}
CComDeviceCamera::~CComDeviceCamera() {
	camera->Close();
}

void CComDeviceCamera::init_task(void* pobj) {

	PylonInitialize();

	//�^�X�N�^�u�E�B���h�E�̕\�������ݒ�
	set_panel_tip_txt();
	
	//Define the video file name.
	string videoFileName = "c:/work/openCvVideo.avi";
	cvVideoCreator.open(videoFileName, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(640, 480), true);

	return;
};

void CComDeviceCamera::routine_work(void* param) {
	
	UINT32 setFrameRate;
	m_cSharedData->GetParam(PARAM_ID_CAM_FRAMERATE, &setFrameRate);
	inf.cycle_ms = 1000 / setFrameRate;
	//�N�������J�E���g�l
	if (inf.cycle_ms >= SYSTEM_TICK_ms)	inf.cycle_count = inf.cycle_ms / SYSTEM_TICK_ms;
	else inf.cycle_count = 1;

	UINT32 camProc, imageProc;
	if (m_cSharedData->GetParam(PARAM_ID_CAM_PROC, &camProc) != RESULT_OK) {
		return;
	}
	if (m_cSharedData->GetParam(PARAM_ID_PIC_PROC_FLAG, &imageProc) != RESULT_OK) {
		return;
	}
	if (camProc == TRUE) {
		CameraCaptureStart();
	}
	else if (imageProc == TRUE){
		ImageProcStart();
	}
	else {
	}

};

LRESULT CALLBACK CComDeviceCamera::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {
	return 0;
};

void CComDeviceCamera::set_panel_tip_txt()
{

}

///# �֐�: �J�����B�e���� ***************
static void CameraCaptureStart(void) {
	CHelper tool;
	wstring out_string;	wstring wstr; string str;

	// �������Ԍv��(�J�n���Ԏ擾)
	LARGE_INTEGER    frequency;
	QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER    start;
	QueryPerformanceCounter(&start);

	if (camera == NULL) {
		try
		{
			camera = new CBaslerUniversalInstantCamera(CTlFactory::GetInstance().CreateFirstDevice());
		}
		catch (GenICam::GenericException & e) {// �G���[�n���h�����O
			//str = e.GetDescription(); tool.MbStrTowStr(str, wstr);
			//out_string.clear(); out_string = out_string + L"Exception" + wstr;
			//tool.tx_skip_row(1); tool.my_txout(out_string);
			return;
		}
	}

	try {
		// PC�ɐڑ�����Ă���J�����̂����ŁA���߂Ɍ��o���ꂽ���̂ɑ΂��ăJ�����̃C���X�^���X���쐬���܂��B
//		CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
//		CBaslerUniversalInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());

		// �J�����̃��f���������C���E�B���h�E�ɕ\�����܂��B
		//str = camera.GetDeviceInfo().GetModelName(); tool.MbStrTowStr(str, wstr);
		//out_string.clear(); out_string = out_string + L"Using device " + wstr;
		//tool.tx_skip_row(1); tool.my_txout(out_string);

		// �J�����p�����[�^�ɃA�N�Z�X���邽�߂̃J�����m�[�h�}�b�v���擾���܂�
		INodeMap& nodemap = camera->GetNodeMap();
		// �p�����[�^�ɃA�N�Z�X����O�ɃJ�������J���܂�
		camera->Open();

		UINT32 setWidth, setHeight, setExposure, setFrameRate;
		m_cSharedData->GetParam(PARAM_ID_CAM_WIDTH, &setWidth);
		m_cSharedData->GetParam(PARAM_ID_CAM_HEIGHT, &setHeight);
		m_cSharedData->GetParam(PARAM_ID_CAM_EXPOSURE_TIME, &setExposure);
		m_cSharedData->GetParam(PARAM_ID_CAM_FRAMERATE, &setFrameRate);

		camera->Width.SetValue(setWidth);
		camera->Height.SetValue(setHeight);
		//�J�����̕��ƍ����ɃA�N�Z�X���邽�߂̃|�C���^�[�����܂�
		CIntegerPtr width = nodemap.GetNode("Width");
		CIntegerPtr height = nodemap.GetNode("Height");
		camera->ExposureTime.SetValue(setExposure);

		UINT frameRate = (UINT)camera->ResultingFrameRate.GetValue();
		if (setFrameRate < frameRate) {
			m_cSharedData->SetParam(PARAM_ID_CAM_READ_FRAMERATE, setFrameRate);
		}
		else {
			/* �ŏ��l�Ɋۂߍ��� */
			if (frameRate == 0) frameRate = 1;
			m_cSharedData->SetParam(PARAM_ID_CAM_READ_FRAMERATE, frameRate);
		}

		// �摜��荞�ݗp�̃o�b�t�@����5�Ɏw�肵�܂��B������ɖ��m�Ȏw�肪�Ȃ��ꍇ�ɂ́A�f�t�H���g��10���ɂȂ�܂��B
		camera->MaxNumBuffer = 5;

		//pylon ImageFormatConverter object���N���G�C�g
		CImageFormatConverter formatConverter;
		//Specify the output pixcel format.
		formatConverter.OutputPixelFormat = PixelType_BGR8packed;
		//Create a PylonImage that will be used to reate OpenCV images later.
		CPylonImage pylonImage;
		//Declare an integer variable to count the number of grabbed images and create image file names with ascending number.
		static int grabbedImages = 0;

		//Create an OpenCV image.
		Mat openCvImage;
		//Define the video frame size.
		cv::Size frameSize = Size((int)width->GetValue(), (int)height->GetValue());
		//Set the codec type and the frame rate. 3 codec options here
		//The frame rate should match or be lower than the camera acquisition frame rate.

		//Start the grabbing of c_countOfImagesToGrab images.
		//The camera device is parameterized with a default configuration which sets up free-running continuous acquisition.
		camera->StartGrabbing(c_countOfImagesToGrab, GrabStrategy_LatestImageOnly);

		// �摜�f�[�^�擾�p�̃|�C���^��錾���܂��B
		CGrabResultPtr ptrGrabResult;
		//Camera.StopGrabbing() is called automatically by the Retriveresult() method when c_countOfImagesToGrab images hav been retrived.
		while (camera->IsGrabbing()) {
			//Wait for an image and then retrive it. A timeout of 5000 ms is used
			camera->RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

			if (ptrGrabResult->GrabSucceeded())// �摜������Ɏ擾����Ă��邩�m�F���܂�
			{
				// �摜�f�[�^�ɒ��ڃA�N�Z�X���܂��B
				const uint8_t* pImageBuffer = (uint8_t*)ptrGrabResult->GetBuffer();
				out_string.clear();
				out_string = out_string + L"Intensity of the first pixel:" + to_wstring((uint32_t)pImageBuffer[0])
					+ L"   SixeX:" + to_wstring(ptrGrabResult->GetWidth())
					+ L"   SixeY:" + to_wstring(ptrGrabResult->GetHeight());
				//tool.tx_skip_row(1); tool.my_txout(out_string);

				//Pylon::DisplayImage(1, ptrGrabResult);// �摜��\�����܂��B�\���ɂ��Ă�Pylon SDK���񋟂���\���E�C���h�E�𗘗p���܂��B

				//Convert the grabbed buffer to a pylon image.
				formatConverter.Convert(pylonImage, ptrGrabResult);
				//Create an OpenCV image from a pylon image.
				openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());
				if (m_bSaveImageArea == FALSE) {
					m_cSharedData->SetImage(IMAGE_ID_CAM_A, openCvImage);
					m_bSaveImageArea = TRUE;
				}
				else {
					m_cSharedData->SetImage(IMAGE_ID_CAM_B, openCvImage);
					m_bSaveImageArea = FALSE;
				}
				//Set saveImages to '1' to save images
				if (saveImages) {
					//Create the current image name for saving.
					ostringstream s;
					//Create image name files with ascending grabbed image numbers.
					s << "c:/work/image_" << grabbedImages << ".jpg";
					std::string imageName(s.str());
					//Save an OpenCV image.
					imwrite(imageName, openCvImage);
					grabbedImages++;
				}
				//Set recordVideo  to '1' to record AVI video file
				if (recordVideo) {
					//Create an OpenCV video creater.
					//VideoWriter cvVideoCreator;
					//Define the video file name.
					//string videoFileName = "c:/work/openCvVideo.avi";
					//cvVideoCreator.open(videoFileName, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, frameSize, true);
					//cvVideoCreator.open(videoFileName, CV_FOURCC('M', 'P', '4', '2'), 20, frameSize, true);
					//cvVideoCreator.open(videoFileName, CV_FOURCC('M', 'J', 'P', 'G'), 20, frameSize, true);
					if (cvVideoCreator.isOpened()) {
						cvVideoCreator << openCvImage;
					}
				}
				//Create an OpenCV display window.
//				namedWindow("OpenCV Display Window", WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);//other options:CV_AUTOSIZE,CV_FREERATIO

																	   //Display the current image in the OpenCV display window.
//				imshow("OpenCV Display Window", openCvImage);

//				waitKey(0);
			}
			else
			{
				//str = camera.GetDeviceInfo().GetModelName(); tool.MbStrTowStr(str, wstr);
				//out_string.clear(); out_string = out_string + L"�G���[: " + to_wstring(ptrGrabResult->GetErrorCode());
				//tool.tx_skip_row(1); tool.my_txout(out_string);
			}
		}
		// �������Ԍv��(�I�����Ԏ擾)
		LARGE_INTEGER    end;
		QueryPerformanceCounter(&end);
		LONGLONG span = end.QuadPart - start.QuadPart;
		LONGLONG usec = (span * 1000000L) / frequency.QuadPart;

		camera->Close();
	}
	catch (GenICam::GenericException & e) {// �G���[�n���h�����O
		str = e.GetDescription(); 
		//tool.MbStrTowStr(str, wstr);
		//out_string.clear(); out_string = out_string + L"Exception" + wstr;
		//tool.tx_skip_row(1); tool.my_txout(out_string);
		//�ُ펞�̓C���X�^���X���Đ���
		if (camera->IsOpen()) {
			camera->Close();
		}
		camera = NULL;
	}
}

///# �֐�: �摜�t�@�C������ ***************
static void ImageProcStart(void) {
	m_cSharedData->SetParam(PARAM_ID_PIC_PROC_FLAG, (UINT32)FALSE);

	string fileName;
	Mat fileData;
	m_cSharedData->GetParam(PARAM_ID_STR_PROC_FILENAME, &fileName);

	fileData = imread(fileName);

	if (m_bSaveImageArea == FALSE) {
		m_cSharedData->SetImage(IMAGE_ID_CAM_A, fileData);
		m_bSaveImageArea = TRUE;
	}
	else {
		m_cSharedData->SetImage(IMAGE_ID_CAM_B, fileData);
		m_bSaveImageArea = FALSE;
	}
}