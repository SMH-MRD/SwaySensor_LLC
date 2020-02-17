// SwingSensor.cpp: アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "SwingSensor.h"

#include "CTaskObj.h"		//タスクスレッドのクラス
#include "Helper.h"			//補助関数集合クラス

#include <windowsx.h> //コモンコントロール用
#include <commctrl.h> //コモンコントロール用
#include <shlwapi.h>

using namespace std;

#define MAX_LOADSTRING 100

enum {
	INITYPE_CHAR = 0,
	INITYPE_INT,
	INITYPE_SHORT,
	INITYPE_LONG,
	INITYPE_FLOAT,
	INITYPE_DOUBLE
};

// ------------------ パラメータIniのセクション名 ---------------------------
#define	INI_SCT_CAMERA				L"CAMERA"							// セクション名：カメラ
#define	INI_SCT_OPENCV				L"OPENCV"							// セクション名：OPENCV
#define	INI_SCT_RIO					L"RIO"								// セクション名：RIO

// ------------------ カメラ設定セクションのキー名 ---------------------------
#define	INI_KEY_CAM_EXPOSURE		L"ExposureTime"						// キー名：露光時間
#define	INI_KEY_CAM_WIDTH			L"Width"							// キー名：カメラ撮影横幅
#define	INI_KEY_CAM_HEIGHT			L"Height"							// キー名：カメラ撮影高さ
#define	INI_KEY_CAM_FRAMERATE		L"FrameRate"						// キー名：フレームレート

// ------------------ OPENCV設定セクションのキー名 ---------------------------
#define	INI_KEY_OPENCV_MASK1_EN		L"Mask1En"							// キー名：OpenCVマスク1有効無効
#define	INI_KEY_OPENCV_MASK1_MIN	L"Mask1Min"							// キー名：OpenCVマスク1最小値
#define	INI_KEY_OPENCV_MASK1_MAX	L"Mask1Max"							// キー名：OpenCVマスク1最大値
#define	INI_KEY_OPENCV_MASK2_EN		L"Mask2En"							// キー名：OpenCVマスク2有効無効
#define	INI_KEY_OPENCV_MASK2_MIN	L"Mask2Min"							// キー名：OpenCVマスク2最小値
#define	INI_KEY_OPENCV_MASK2_MAX	L"Mask2Max"							// キー名：OpenCVマスク2最大値
#define	INI_KEY_OPENCV_MASK3_EN		L"Mask3En"							// キー名：OpenCVマスク3有効無効
#define	INI_KEY_OPENCV_MASK3_MIN	L"Mask3Min"							// キー名：OpenCVマスク3最小値
#define	INI_KEY_OPENCV_MASK3_MAX	L"Mask3Max"							// キー名：OpenCVマスク3最大値
#define	INI_KEY_OPENCV_PROC_ALGO	L"ProcAlgo"							// キー名：OpenCVアルゴリズム

// ------------------ RIO設定セクションのキー名 ---------------------------
#define INI_KEY_RIO_IPADDR			L"IpAddr"							// キー名:RIO IPアドレス
#define INI_KEY_RIO_TCPPORTNUM		L"TcpPort"							// キー名:RIO TCPポート番号
#define INI_KEY_RIO_SLAVEADDR		L"SlaveAddr"						// キー名:RIOスレーブアドレス
#define INI_KEY_RIO_TIMEOUT			L"TimeOut"							// キー名:RIOタイムアウト
#define INI_KEY_RIO_XPORTNUM		L"XPortNum"							// キー名:RIO傾斜計Xデータ接続ポート番号
#define INI_KEY_RIO_YPORTNUM		L"YPortNum"							// キー名:RIO傾斜計Yデータ接続ポート番号

// ------------------ Iniファイル情報の構造体 ---------------------------
typedef struct _stIniInf				//***** Iniファイル情報 *****
{
	INT exposureTime;					// 露光時間
	INT camWidth;						// カメラ取得横幅
	INT camHeight;						// カメラ取得高さ
	INT frameRate;						// フレームレート
	INT mask1En;						// 映像処理マスク1有効無効
	INT mask1Min;						// 映像処理マスク1最小
	INT mask1Max;						// 映像処理マスク1最大
	INT mask2En;						// 映像処理マスク2有効無効
	INT mask2Min;						// 映像処理マスク2最小
	INT mask2Max;						// 映像処理マスク2最大
	INT mask3En;						// 映像処理マスク3有効無効
	INT mask3Min;						// 映像処理マスク3最小
	INT mask3Max;						// 映像処理マスク3最大
	INT procAlgo;						// 画像処理アルゴリズム
	TCHAR rioIpAddr[256];				// RIO IPアドレス
	INT rioTcpPortNum;					// RIO TCPポート番号
	INT rioSlaveAddr;					// RIOスレーブアドレス
	INT rioTimeOut;						// RIOタイムアウト
	INT rioXPortNum;					// RIO傾斜計X軸データ接続ポート番号
	INT rioYPortNum;					// RIO傾斜計Y軸データ接続ポート番号
} ST_INI_INF;

///*****************************************************************************************************************
/// #ここにアプリケーション専用の関数を追加しています。

VOID	CALLBACK alarmHandlar(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);	//マルチメディアタイマ処理関数　スレッドのイベントオブジェクト処理
int		Init_tasks();	//アプリケーション毎のタスクオブジェクトの初期設定
DWORD	knlTaskStartUp();	//実行させるタスクの起動処理
INT		setParameter(ST_INI_INF* pInf, LPCWSTR pFileName);
void	GetIniInf(LPCWSTR file_name, LPCWSTR section_name, LPCWSTR key_name, LPCWSTR str_default, int value_type, void* p_param);
void	CreateSharedData(void);

static unsigned __stdcall thread_gate_func(void * pObj) { //スレッド実行のためのゲート関数
	CTaskObj * pthread_obj = (CTaskObj *)pObj;  
	return pthread_obj->run(pObj); 
}
///*****************************************************************************************************************
/// #ここにアプリケーション専用のグローバル変数　または、スタティック変数を追加しています。
// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名
SYSTEMTIME gAppStartTime;						//アプリケーション開始時間
LPWSTR pszInifile;								// iniファイルのパス
wstring wstrPathExe;							// 実行ファイルのパス

vector<void*>	VectpCTaskObj;	//タスクオブジェクトのポインタ
ST_iTask g_itask;

// スタティック変数:
static HWND hTabWnd;//操作パネル用タブコントロールウィンドウのハンドル
static ST_KNL_MANAGE_SET knl_manage_set;//マルチスレッド管理用構造体
static vector<HANDLE>	VectHevent;		//マルチスレッド用イベントのハンドル

static HIMAGELIST	hImgListTaskIcon;	//タスクアイコン用イメージリスト

static CSharedData*		cSharedData;	// 共有オブジェクトインスタンス

///*****************************************************************************************************************

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);


// # 関数: wWinMain ************************************
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化しています。
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SWINGSENSOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーションの初期化を実行します:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SWINGSENSOR));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

// # 関数: MyRegisterClass ウィンドウ クラスを登録します。***
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SWINGSENSOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SWINGSENSOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// # 関数: InitInstance インスタンス ハンドルを保存して、メイン ウィンドウを作成します。***
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   GetSystemTime(&gAppStartTime);//開始時刻取得


//   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
//	   MAIN_WND_INIT_POS_X, MAIN_WND_INIT_POS_Y,
//	   TAB_DIALOG_W + 40, (MSG_WND_H + MSG_WND_Y_SPACE)*TASK_NUM + TAB_DIALOG_H + 110,
//	   nullptr, nullptr, hInstance, nullptr);

//   if (!hWnd) return FALSE;

//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);

   /// -タスク設定	
   Init_tasks();//タスク個別設定

  /// -タスク起動									
   knlTaskStartUp(); 

   /// -マルチメディアタイマ起動
   {
	/// > マルチメディアタイマ精度設定
	TIMECAPS wTc;//マルチメディアタイマ精度構造体
	if (timeGetDevCaps(&wTc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 	return((DWORD)FALSE);
	 knl_manage_set.mmt_resolution = MIN(MAX(wTc.wPeriodMin, TARGET_RESOLUTION), wTc.wPeriodMax);
	if (timeBeginPeriod(knl_manage_set.mmt_resolution) != TIMERR_NOERROR) return((DWORD)FALSE);

	 _RPT1(_CRT_WARN, "MMTimer Period = %d\n", knl_manage_set.mmt_resolution);

	 /// > マルチメディアタイマセット
	 knl_manage_set.KnlTick_TimerID = timeSetEvent(knl_manage_set.cycle_base, knl_manage_set.mmt_resolution, (LPTIMECALLBACK)alarmHandlar, 0, TIME_PERIODIC);

	 /// >マルチメディアタイマー起動失敗判定　メッセージBOX出してFALSE　returen
	 if (knl_manage_set.KnlTick_TimerID == 0) {	 //失敗確認表示
	   LPVOID lpMsgBuf;
	   FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		   0, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language*/(LPTSTR)&lpMsgBuf, 0, NULL);
	   MessageBox(NULL, (LPCWSTR)lpMsgBuf, L"MMT Failed!!", MB_OK | MB_ICONINFORMATION);// Display the string.
	   LocalFree(lpMsgBuf);// Free the buffer.
	   return((DWORD)FALSE);
	}
   }
   return TRUE;
}

// # 関数: メイン ウィンドウのメッセージを処理します。*********************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
        }break;
	case WM_CREATE: {
		}break;

	case WM_PAINT:{
        }break;
	case WM_SIZE:{
	}break;

    case WM_DESTROY:
		delete cSharedData;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

///# 関数: スレッドタスクの登録、設定 ***
int  Init_tasks() {
	HBITMAP hBmp;
	CTaskObj *ptempobj;
	int task_index = 0;

	InitCommonControls();//コモンコントロール初期化
	hImgListTaskIcon = ImageList_Create(32, 32, ILC_COLOR | ILC_MASK, 2, 0);//タスクアイコン表示用イメージリスト設定

	CreateSharedData();

	//###Task1 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CManager;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.mng = task_index;


		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;


		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

		/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 1000;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_MAN1");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255)); 
		DeleteObject(hBmp);

		 ///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, MANAGER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
				str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, MANAGER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
				ptempobj->inf.work_select = THREAD_WORK_ROUTINE;
		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
				ptempobj->inf.n_active_events = 1;
	
	}
	//###Task2 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CPlayer;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.ply = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																						   /// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 50;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_PLY1");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, PLAYER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, PLAYER_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}
	

	//###Task3 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CComClient;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.comc = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 1000;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_CCOM");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, CLIENT_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, CLIENT_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	//###Task4 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CComDeviceRIO;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.comp = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 25;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_PCOM");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, DEVICE_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, DEVICE_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	//###Task5 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CComDeviceCamera;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.comp = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 50;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_PCOM");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, DEVICE_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, DEVICE_COM_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	//###Task6 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CPublicRelation(hInst);
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.pr = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 50;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_PR");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, PR_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, PR_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	//###Task7 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CClerk;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.clerk = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 1000;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_CLERK");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, CLERK_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, CLERK_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	//###Task8 設定
	{
		/// -タスクインスタンス作成->リスト登録
		ptempobj = new CAnalyst;
		VectpCTaskObj.push_back((void*)ptempobj);
		g_itask.ana = task_index;

		/// -タスクインデクスセット
		ptempobj->inf.index = task_index++;

		/// -イベントオブジェクトクリエイト->リスト登録	
		VectHevent.push_back(ptempobj->inf.hevents[ID_TIMER_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL));//自動リセット,初期値非シグナル

																											/// -スレッド起動周期セット
		ptempobj->inf.cycle_ms = 50;

		/// -ツイートメッセージ用iconセット
		hBmp = (HBITMAP)LoadBitmap(hInst, L"IDB_ANALYST");//ビットマップ割り当て
		ImageList_AddMasked(hImgListTaskIcon, hBmp, RGB(255, 255, 255));
		DeleteObject(hBmp);

		///オブジェクト名セット
		DWORD	str_num = GetPrivateProfileString(OBJ_NAME_SECT_OF_INIFILE, ANALYST_KEY_OF_INIFILE, L"No Name", ptempobj->inf.name, sizeof(ptempobj->inf.name) / 2, PATH_OF_INIFILE);
		str_num = GetPrivateProfileString(OBJ_SNAME_SECT_OF_INIFILE, ANALYST_KEY_OF_INIFILE, L"No Name", ptempobj->inf.sname, sizeof(ptempobj->inf.sname) / 2, PATH_OF_INIFILE);

		///実行関数選択
		ptempobj->inf.work_select = THREAD_WORK_ROUTINE;

		///スレッド起動に使うイベント数（定周期タイマーのみの場合１）
		ptempobj->inf.n_active_events = 1;

	}

	///各タスク用設定ウィンドウ作成
	InitCommonControls();	//コモンコントロール初期化
//	hTabWnd = CreateTaskSettingWnd(hWnd);//タブウィンドウ作成
	
	//各タスク残パラメータセット
	knl_manage_set.num_of_task = (unsigned int)VectpCTaskObj.size();//タスク数登録
	for (unsigned i = 0; i < knl_manage_set.num_of_task; i++) {
		CTaskObj * pobj = (CTaskObj *)VectpCTaskObj[i];
	
		pobj->inf.index = i;	//タスクインデックスセット
		
		//その他設定
		pobj->inf.psys_counter = &knl_manage_set.sys_counter;
		pobj->inf.act_count = 0;//起動チェック用カウンタリセット
		 //起動周期カウント値
		if (pobj->inf.cycle_ms >= SYSTEM_TICK_ms)	pobj->inf.cycle_count = pobj->inf.cycle_ms / SYSTEM_TICK_ms;
		else pobj->inf.cycle_count = 1;

		//最後に初期化関数呼び出し
		pobj->init_task(pobj);
	}

	return 1;
}

///# 関数: マルチタスクスタートアップ処理関数 ***
DWORD knlTaskStartUp()	//実行させるオブジェクトのリストのスレッドを起動
{
	//機能	：[KNL]システム/ユーザタスクスタートアップ関数
	//処理	：自プロセスのプライオリティ設定，カーネルの初期設定,タスク生成，基本周期設定
	//戻り値：Win32APIエラーコード
	
	HANDLE	myPrcsHndl;	/* 本プログラムのプロセスハンドル */
						///# 自プロセスプライオリティ設定処理
						//-プロセスハンドル取得
	if ((myPrcsHndl = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION, FALSE, _getpid())) == NULL)	return(GetLastError());
	_RPT1(_CRT_WARN, "KNL Priority For Windows(before) = %d \n", GetPriorityClass(myPrcsHndl));

	//-自プロセスのプライオリティを最優先ランクに設定
	if (SetPriorityClass(myPrcsHndl, REALTIME_PRIORITY_CLASS) == 0)	return(GetLastError());
	_RPT1(_CRT_WARN, "KNL Priority For NT(after) = %d \n", GetPriorityClass(myPrcsHndl));

	///# アプリケーションタスク数が最大数を超えた場合は終了
	if (VectpCTaskObj.size() >= MAX_APL_TASK)	return((DWORD)ERROR_BAD_ENVIRONMENT);

	///#    アプリケーションスレッド生成処理	
	for (unsigned i = 0; i < VectpCTaskObj.size(); i++) {

		CTaskObj * pobj = (CTaskObj *)VectpCTaskObj[i];

		// タスク生成(スレッド生成)
		// 他ﾌﾟﾛｾｽとの共有なし,スタック初期サイズ　デフォルト, スレッド実行関数　引数で渡すオブジェクトで対象切り替え,スレッド関数の引数（対象のオブジェクトのポインタ）, 即実行Createflags, スレッドID取り込み
		pobj->inf.hndl = (HANDLE)_beginthreadex((void *)NULL, 0, thread_gate_func, VectpCTaskObj[i], (unsigned)0, (unsigned *)&(pobj->inf.ID));

		// タスクプライオリティ設定
		if (SetThreadPriority(pobj->inf.hndl, pobj->inf.priority) == 0)
			return(GetLastError());
		_RPT2(_CRT_WARN, "Task[%d]_priority = %d\n", i, GetThreadPriority(pobj->inf.hndl));

		pobj->inf.act_count = 0;		// 基本ティックのカウンタ変数クリア
		pobj->inf.time_over_count = 0;	// 予定周期オーバーカウントクリア
	}

	return 1;
}

///# 関数: マルチメディアタイマーイベント処理関数 ****************************************************
VOID CALLBACK alarmHandlar( UINT uID, UINT uMsg, DWORD	dwUser,	DWORD dw1, DWORD dw2){

	LONG64 tmttl;
	knl_manage_set.sys_counter++;

	//スレッド再開イベントセット処理
	for (unsigned i = 0; i < knl_manage_set.num_of_task; i++) {
		CTaskObj * pobj = (CTaskObj *)VectpCTaskObj[i];
		pobj->inf.act_count++;
		if (pobj->inf.act_count >= pobj->inf.cycle_count) {
			PulseEvent(VectHevent[i]);
			pobj->inf.act_count = 0;
			pobj->inf.total_act++;
		}
	}
}

///# 関数: 初期設定ファイル読出し ***************
INT setParameter(ST_INI_INF* pInf, LPCWSTR pFileName)
{
	//--------------------------------------------------------------------------
	// 設定ファイル存在チェック
	if (!(PathFileExists(pFileName)) || PathIsDirectory(pFileName))
	{
		return RESULT_NG_INVALID;
	}

	//--------------------------------------------------------------------------
	// 共通設定パラメータ
	GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_EXPOSURE,
		L"10000", INITYPE_INT, &(pInf->exposureTime));				// 露光時間
	GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_WIDTH,
		L"640", INITYPE_INT, &(pInf->camWidth));					// カメラ撮影横幅
	GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_HEIGHT,
		L"480", INITYPE_INT, &(pInf->camHeight));					// カメラ撮影高さ
	GetIniInf(pFileName, INI_SCT_CAMERA, INI_KEY_CAM_FRAMERATE,
		L"30", INITYPE_INT, &(pInf->frameRate));					// フレームレート

	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK1_EN,
		L"1", INITYPE_INT, &(pInf->mask1En));						// マスク1有効無効
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK1_MIN,
		L"0", INITYPE_INT, &(pInf->mask1Min));						// マスク1最小
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK1_MAX,
		L"10", INITYPE_INT, &(pInf->mask1Max));						// マスク1最大
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK2_EN,
		L"1", INITYPE_INT, &(pInf->mask2En));						// マスク2有効無効
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK2_MIN,
		L"170", INITYPE_INT, &(pInf->mask2Min));					// マスク2最小
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK2_MAX,
		L"180", INITYPE_INT, &(pInf->mask2Max));					// マスク2最大
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK3_EN,
		L"1", INITYPE_INT, &(pInf->mask3En));						// マスク3有効無効
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK3_MIN,
		L"80", INITYPE_INT, &(pInf->mask3Min));						// マスク3最小
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_MASK3_MAX,
		L"100", INITYPE_INT, &(pInf->mask3Max));					// マスク3最大
	GetIniInf(pFileName, INI_SCT_OPENCV, INI_KEY_OPENCV_PROC_ALGO,
		L"100", INITYPE_INT, &(pInf->procAlgo));					// 画像処理アルゴリズム

	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_IPADDR,
		L"192.168.0.1", INITYPE_CHAR, &(pInf->rioIpAddr));			// RIO IPアドレス
	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_TCPPORTNUM,
		L"502", INITYPE_INT, &(pInf->rioTcpPortNum));				// RIO TCPポート番号
	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_SLAVEADDR,
		L"1", INITYPE_INT, &(pInf->rioSlaveAddr));					// RIOスレーブアドレス
	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_TIMEOUT,
		L"2000", INITYPE_INT, &(pInf->rioTimeOut));					// RIOタイムアウト
	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_XPORTNUM,
		L"1", INITYPE_INT, &(pInf->rioXPortNum));					// RIO傾斜計Xデータ接続ポート番号
	GetIniInf(pFileName, INI_SCT_RIO, INI_KEY_RIO_YPORTNUM,
		L"2", INITYPE_INT, &(pInf->rioYPortNum));					// RIO傾斜計Yデータ接続ポート番号

	return S_OK;
}

///# 関数: INIファイル読出し ***************
void GetIniInf(LPCWSTR file_name, LPCWSTR section_name, LPCWSTR key_name, LPCWSTR str_default, int value_type, void* p_param)
{
	DWORD	state;
	WCHAR	buf[256];

	state = GetPrivateProfileString(section_name, key_name, str_default, buf, sizeof(buf) / 2, file_name);
	if (state <= 0)
	{
		_stprintf_s(buf, L"%s", str_default);
	}

	switch (value_type)
	{
	case INITYPE_CHAR:		_stprintf_s((LPWSTR)p_param, 256, buf);	break;
	case INITYPE_INT:		*((int*)p_param) = _wtoi(buf);			break;
	case INITYPE_SHORT:		*((short*)p_param) = (short)_wtoi(buf);	break;
	case INITYPE_LONG:		*((long*)p_param) = _wtoi(buf);			break;
	case INITYPE_FLOAT:		*((float*)p_param) = (float)_wtof(buf);	break;
	case INITYPE_DOUBLE:	*((double*)p_param) = _wtof(buf);		break;
	default:														break;
	}
}

///# 関数: 共有オブジェクト初期化 ***************
void CreateSharedData(void) {
	// ini file path
	static WCHAR dstpath[_MAX_PATH], szDrive[_MAX_DRIVE], szPath[_MAX_PATH], szFName[_MAX_FNAME], szExt[_MAX_EXT];
	GetModuleFileName(NULL, dstpath, (sizeof(TCHAR) * _MAX_PATH) / 2);
	_wsplitpath_s(dstpath, szDrive, sizeof(szDrive) / 2, szPath, sizeof(szPath) / 2, szFName, sizeof(szFName) / 2, szExt, sizeof(szExt) / 2);

	wstrPathExe = szDrive; wstrPathExe += szPath;

	_wmakepath_s(dstpath, sizeof(dstpath) / 2, szDrive, szPath, NAME_OF_INIFILE, EXT_OF_INIFILE);
	pszInifile = dstpath;

	ST_INI_INF ini;
	setParameter(&ini, dstpath);

	cSharedData = new CSharedData();
	cSharedData->InitSharedData();

	cSharedData->SetParam(PARAM_ID_CAM_EXPOSURE_TIME, (UINT32)ini.exposureTime);
	cSharedData->SetParam(PARAM_ID_CAM_FRAMERATE, (UINT32)ini.frameRate);
	cSharedData->SetParam(PARAM_ID_CAM_WIDTH, (UINT32)ini.camWidth);
	cSharedData->SetParam(PARAM_ID_CAM_HEIGHT, (UINT32)ini.camHeight);

	cSharedData->SetParam(PARAM_ID_PIC_HUE1_EN, (UINT32)ini.mask1En);
	cSharedData->SetParam(PARAM_ID_PIC_HUE1_MIN, (UINT32)ini.mask1Min);
	cSharedData->SetParam(PARAM_ID_PIC_HUE1_MAX, (UINT32)ini.mask1Max);
	cSharedData->SetParam(PARAM_ID_PIC_HUE2_EN, (UINT32)ini.mask2En);
	cSharedData->SetParam(PARAM_ID_PIC_HUE2_MIN, (UINT32)ini.mask2Min);
	cSharedData->SetParam(PARAM_ID_PIC_HUE2_MAX, (UINT32)ini.mask2Max);
	cSharedData->SetParam(PARAM_ID_PIC_HUE3_EN, (UINT32)ini.mask3En);
	cSharedData->SetParam(PARAM_ID_PIC_HUE3_MIN, (UINT32)ini.mask3Min);
	cSharedData->SetParam(PARAM_ID_PIC_HUE3_MAX, (UINT32)ini.mask3Max);
	cSharedData->SetParam(PARAM_ID_PIC_COG_ALGO, (UINT32)ini.procAlgo);

	char* cstr = (char*)malloc(sizeof(ini.rioIpAddr));
	if (cstr != NULL) {
		size_t size;
		wcstombs_s(&size, cstr, sizeof(ini.rioIpAddr), ini.rioIpAddr, sizeof(ini.rioIpAddr));
		string str = cstr;
		cSharedData->SetParam(PARAM_ID_STR_RIO_IPADDR, str);
		free(cstr);
	}
	cSharedData->SetParam(PARAM_ID_RIO_TCPPORT, (UINT32)ini.rioTcpPortNum);
	cSharedData->SetParam(PARAM_ID_RIO_SLAVEADDR, (UINT32)ini.rioSlaveAddr);
	cSharedData->SetParam(PARAM_ID_RIO_TIMEOUT, (UINT32)ini.rioTimeOut);
	cSharedData->SetParam(PARAM_ID_RIO_XPORT, (UINT32)ini.rioXPortNum);
	cSharedData->SetParam(PARAM_ID_RIO_YPORT, (UINT32)ini.rioYPortNum);
}



