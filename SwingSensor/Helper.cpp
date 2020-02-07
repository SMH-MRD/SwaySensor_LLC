#include "stdafx.h"
#include "Helper.h"

CHelper::CHelper(){}

CHelper::~CHelper(){}

/*****************************************************************************
string型をwstring型に変換します
- 引数　const std::string &src：変換前入力　, std::wstring &dest：変換後出力　
- 戻り値　NA
*****************************************************************************/
void CHelper::Str2Wstr(const std::string &src, std::wstring &dest) {
	
	setlocale(LC_ALL, "");
	
	wchar_t *wcs = new wchar_t[src.length() + 1];
	
	size_t ret_val;
	mbstowcs_s(&ret_val, wcs, src.length() + 1, src.c_str(), _TRUNCATE);
	
	dest = wcs;delete[] wcs;
	return;
}

/*****************************************************************************************
指定されたDC、位置に指定されたビットマップを書き込む（等倍）
- 引数	HDC hdc：書込先DC, HBITMAP hbmp：素材ビットマップ,
		POINT dst_pt：書込先の位置, POINT src_pt：書込元の位置
- 戻り値　NA
******************************************************************************************/
void CHelper::put_bmp_built(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt) {

	if ((hdc == NULL) | (hbmp == NULL)) return;

	BITMAP bmp;
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	int BMP_W = (int)bmp.bmWidth;
	int BMP_H = (int)bmp.bmHeight;

	HDC hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hbmp);

	BitBlt(hdc, dst_pt.x, dst_pt.y, BMP_W, BMP_H, hmdc, src_pt.x, src_pt.y, SRCCOPY);
	DeleteDC(hmdc);

	return;
};

/******************************************************************************************************
指定されたDC、位置に指定されたビットマップを書き込む（変倍）
- 引数	HDC hdc：書込先DC, HBITMAP hbmp：素材ビットマップ,
		POINT dst_pt：書込先の位置, POINT src_pt：書込元の位置,int retio_percent:書込先倍率
- 戻り値	NA
******************************************************************************************************/
void CHelper::put_bmp_stretch(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt, int retio_percent) {

	if ((hdc == NULL) | (hbmp == NULL)) return;

	BITMAP bmp;
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	int BMP_W = (int)bmp.bmWidth;
	int BMP_H = (int)bmp.bmHeight;

	HDC hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hbmp);

	StretchBlt(hdc, dst_pt.x, dst_pt.y, BMP_W*retio_percent / 100, BMP_H * retio_percent / 100, hmdc, src_pt.x, src_pt.y, BMP_W, BMP_H, SRCCOPY);
	DeleteDC(hmdc);

	return;
};

/****************************************************************************************************************************************************
共有メモリを確保する
- 引数
	-入力　LPCTSTR szName：共有メモリ名, DWORD dwSize：確保するサイズ,
	-出力　HANDLE* hMapFile：ファイルマップオブジェクトハンドル, LPVOID* pMapTop:共有メモリ先頭アドレス, DWORD* dwExist:　GetLastError()エラー有り

- 戻り値　0:正常完了　-1,-2:異常完了
****************************************************************************************************************************************************/
int CHelper::cmnCreateShmem(LPCTSTR szName, DWORD dwSize, HANDLE* hMapFile, LPVOID* pMapTop, DWORD* dwExist) {
	DWORD	highByte = 0;			// 共有メモリは32bitサイズ以内を想定
	DWORD	lowByte = dwSize;		// 32bitサイズ以上は定義不可

									// 初期化
	*hMapFile = NULL;
	*pMapTop = NULL;
	*dwExist = OFF;

	// ファイル・マッピング・オブジェクトを作成(ページファイルを使用 ,セキュリティ属性なし ,読み込み/書き込みアクセス ,共有メモリのサイズ ,共有メモリ名)
	*hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, highByte, lowByte, szName);
	if (*hMapFile == NULL) return(ERR_SHMEM_CREATE);
	else {
		if (GetLastError() == ERROR_ALREADY_EXISTS)	*dwExist = ON;
		// ファイル・ビューを作成(共有メモリのハンドル, アクセスの種類, ファイル オフセット(上位32ビット), ファイル オフセット(下位32ビット), マップするバイト数→0はファイルﾙマッピングオブジェクト全体)
		*pMapTop = MapViewOfFile(*hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (*pMapTop == NULL) {
			CloseHandle(*hMapFile);	// 共有メモリを閉じる
			*hMapFile = NULL; *pMapTop = NULL;
			return(ERR_SHMEM_VIEW);
		}
	}
	return(OK_SHMEM);
}

/****************************************************************************************************************************************************
共有メモリを開放する
- 引数　HANDLE* hMapFile,：解放先ファイルマップオブジェクト LPVOID* pMapTop：解放先メモリ先頭アドレス
- 戻り値　0:正常完了,-2:異常完了
****************************************************************************************************************************************************/
int	CHelper::cmnDeleteShMem(HANDLE* hMapFile, LPVOID* pMapTop)
{
	int		ret = OK_SHMEM;		// 関数ステータス
								
	if (*pMapTop != NULL) {
		if (!UnmapViewOfFile(*pMapTop)) {	// ファイル・ビューの削除
			ret = ERR_SHMEM_VIEW;
		}
	}

	// ファイル・マッピング・オブジェクトを閉じる
	if (*hMapFile != NULL) {
		CloseHandle(*hMapFile);
	}

	// ポインタ初期化
	*hMapFile = NULL;
	*pMapTop = NULL;

	return(ret);
}

/****************************************************************************************************************************************************
実行ファイルフォルダ内のファイルのパスを作る
- 引数　LPWSTR pst_file_path：パス名格納先バッファ, const wchar_t* filename　：ファイル名, const wchar_t* ext　：ファイル拡張子
- 戻り値　0:正常完了,0以外:異常完了
****************************************************************************************************************************************************/

// ini file path
int	CHelper::create_file_path_of_exe_folder(LPWSTR pst_file_path, const wchar_t* filename, const wchar_t* ext) 
{

	static WCHAR szDrive[_MAX_DRIVE], szPath[_MAX_PATH], szFName[_MAX_FNAME], szExt[_MAX_EXT];

	GetModuleFileName(NULL, pst_file_path, (sizeof(TCHAR) * _MAX_PATH));
	_wsplitpath_s(pst_file_path, szDrive, sizeof(szDrive) / 2, szPath, sizeof(szPath) / 2, szFName, sizeof(szFName) / 2, szExt, sizeof(szExt) / 2);

	wstring wstrPathExe = szDrive; wstrPathExe += szPath;

	_wmakepath_s(pst_file_path, _MAX_PATH, szDrive, szPath, filename, ext);
	
	return 0;
}
#if 1
int CHelper::dispbmp(HWND hWnd, BITMAPINFO bmpIf, PUINT8 lpPix, int com, POINT pt, RECT rio_rect, UINT32 color, float retiofx, float retiofy) {

	HDC hdc;
	PAINTSTRUCT ps;
	LONG width, height;

	width = bmpIf.bmiHeader.biWidth;
	height = bmpIf.bmiHeader.biHeight;
	InvalidateRect(hWnd, NULL, FALSE);
	hdc = BeginPaint(hWnd, &ps);
	RECT rect;
	GetClientRect(hWnd, &rect);

	if (com & DRAW_LINE_MOUSE_PT) {
		put_line(bmpIf, pt, lpPix, color, retiofx, retiofy);
	}
	if (com & DRAW_LINE_DISP_ROI) {
		put_rect(bmpIf, rio_rect, lpPix, 0x00ff00ff, retiofx, retiofy);
	}

	SetStretchBltMode(hdc, COLORONCOLOR);
	StretchDIBits(hdc, 0, HEIGHT_VGA + DISP_OFFSET_Y, WIDTH_VGA, -1 * HEIGHT_VGA,
		0, 0, width, height, lpPix, &bmpIf, DIB_RGB_COLORS, SRCCOPY);


	EndPaint(hWnd, &ps);

	return 0;
};

int CHelper::dispmat(Mat s_mat, HWND hWnd) {

	HDC hdc;
	PAINTSTRUCT ps;
	LONG width, height;
	BITMAPINFO bmpIftemp;
	Mat work_mat;


	width = s_mat.cols;
	height = s_mat.rows;
	bmpIftemp.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpIftemp.bmiHeader.biWidth = (int)width;
	bmpIftemp.bmiHeader.biHeight = (int)height;
	bmpIftemp.bmiHeader.biPlanes = 1;
	bmpIftemp.bmiHeader.biBitCount = 24;
	bmpIftemp.bmiHeader.biCompression = BI_RGB;

	InvalidateRect(hWnd, NULL, FALSE);
	hdc = BeginPaint(hWnd, &ps);
	RECT rect;
	GetClientRect(hWnd, &rect);

	SetStretchBltMode(hdc, COLORONCOLOR);

	if (s_mat.depth() != CV_8U)s_mat.convertTo(work_mat, CV_8U);
	else work_mat = s_mat;

	if (work_mat.channels() == 3) {
		StretchDIBits(hdc, 0, HEIGHT_VGA + DISP_OFFSET_Y, WIDTH_VGA, -1 * HEIGHT_VGA,
			0, 0, width, height, s_mat.data, &bmpIftemp, DIB_RGB_COLORS, SRCCOPY);
	}
	else {
		vector<cv::Mat> gray_mat;
		Mat dst;
		gray_mat.push_back(work_mat);
		gray_mat.push_back(work_mat);
		gray_mat.push_back(work_mat);
		cv::merge(gray_mat, dst);

		StretchDIBits(hdc, 0, HEIGHT_VGA + DISP_OFFSET_Y, WIDTH_VGA, -1 * HEIGHT_VGA,
			0, 0, width, height, dst.data, &bmpIftemp, DIB_RGB_COLORS, SRCCOPY);
	}

	//	StretchDIBits(hdc, 0, HEIGHT_VGA + DISP_OFFSET_Y, WIDTH_VGA, -1 * HEIGHT_VGA,
	//		0, 0, width, height, s_mat.data, &bmpIftemp, DIB_RGB_COLORS, SRCCOPY);

	EndPaint(hWnd, &ps);

	return 0;
};


int CHelper::put_line(BITMAPINFO bmpIf, POINT pt, PUINT8 lpPixcel, UINT32 color, float retiofx, float retiofy) {
	int range_line_y, range_line_x, range_point_y, range_point_x;

	range_line_y = 1 * retiofy; range_line_x = 2 * retiofx; range_point_y = 4 * retiofy; range_point_x = 4 * retiofx;

	for (int i = 0; i < bmpIf.bmiHeader.biHeight; i++)
		for (int j = 0; j < bmpIf.bmiHeader.biWidth * 3; j = j + 3) {
			if (((i < pt.y + range_line_y) && (i > pt.y - 1 * range_line_y) && (j % 120 * retiofx < 90 * retiofx)) || ((j < 3 * pt.x + range_line_x) && (j > 3 * pt.x - 1 * range_line_x) && (i % 40 * retiofy < 30 * retiofy)))
				*(PUINT32)(lpPixcel + j + i * bmpIf.bmiHeader.biWidth * 3)
				= color;
			if (((i < pt.y + range_point_y) && (i > pt.y - 1 * range_point_y) && (j < ((3 * pt.x) + range_point_x)) && (j > ((3 * pt.x) - 1 * range_point_x))))
				*(PUINT32)(lpPixcel + j + i * bmpIf.bmiHeader.biWidth * 3) = 0xffff0000;
		}
	return 1;
};

int CHelper::put_rect(HWND hWnd, RECT rect, UINT32 color) {
#if 0	
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	HPEN hPen = (HPEN)CreatePen(PS_DOT, 1, RGB(255, 0, 255));
	HPEN holdPen = (HPEN)SelectObject(hdc, hPen);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	SelectObject(hdc, holdPen);
	DeleteObject(hPen);
	EndPaint(hWnd, &ps);
#else
	HDC hdc;
	HPEN hPen;

	hdc = GetDC(hWnd);

	hPen = CreatePen(PS_DOT, 3, color);
	SelectObject(hdc, hPen);

	SetROP2(hdc, R2_NOT);
	MoveToEx(hdc, rect.left, rect.top, NULL);
	LineTo(hdc, rect.right, rect.top);
	LineTo(hdc, rect.right, rect.bottom);
	LineTo(hdc, rect.left, rect.bottom);
	LineTo(hdc, rect.left, rect.top);
	ReleaseDC(hWnd, hdc);

	DeleteObject(hPen);
#endif
	return 1;
}

int CHelper::put_rect(BITMAPINFO bmpIf, RECT rect, PUINT8 lpPixcel, UINT32 color, float retiofx, float retiofy) {

	if ((rect.left > bmpIf.bmiHeader.biWidth) | (rect.right > bmpIf.bmiHeader.biWidth) | (rect.top > bmpIf.bmiHeader.biHeight) | (rect.bottom > bmpIf.bmiHeader.biHeight))return 0;

	int dlinex, dliney;
	if ((int)retiofx) dlinex = (int)retiofx * 2; else dlinex = 1;
	if ((int)retiofy) dliney = (int)retiofy * 2; else dliney = 1;


	for (int k = 0; k < (int)dliney; k++) {
		for (int i = rect.left; i < rect.right - 1; i++) *(PUINT32)(lpPixcel + (rect.top + k) * bmpIf.bmiHeader.biWidth * 3 + i * 3) = color;
		for (int i = rect.left; i < rect.right - 1; i++) *(PUINT32)(lpPixcel + (rect.bottom - 1 - k) * bmpIf.bmiHeader.biWidth * 3 + i * 3) = color;
	}

	for (int k = 0; k < (int)dlinex; k++) {
		for (int i = rect.top; i < rect.bottom - 1; i++) *(PUINT32)(lpPixcel + i * bmpIf.bmiHeader.biWidth * 3 + (rect.left + k) * 3) = color;
		for (int i = rect.top; i < rect.bottom - 1; i++) *(PUINT32)(lpPixcel + i * bmpIf.bmiHeader.biWidth * 3 + (rect.right - 1 - k) * 3) = color;
	}

	return 1;
};

void  CHelper::DrawRect(HWND hWnd, POINTS beg, POINTS end) {
	HDC hdc;
	HPEN hPen;

	hdc = GetDC(hWnd);

	hPen = CreatePen(PS_DOT, 1, RGB(0, 255, 0));
	SelectObject(hdc, hPen);

	SetROP2(hdc, R2_NOT);
	MoveToEx(hdc, beg.x, beg.y, NULL);
	LineTo(hdc, end.x, beg.y);
	LineTo(hdc, end.x, end.y);
	LineTo(hdc, beg.x, end.y);
	LineTo(hdc, beg.x, beg.y);
	ReleaseDC(hWnd, hdc);

	DeleteObject(hPen);

	return;
}

void  CHelper::DrawRect(HWND hWnd, Point2f pt1, Point2f pt2, Point2f pt3, Point2f pt4, Point2f pt_center, cv::Size matsize) {

	POINT pt1_0, pt2_0, pt3_0, pt4_0, pt_center_0;
	float retiofx = float(matsize.width) / WIDTH_VGA;
	float retiofy = float(matsize.height) / HEIGHT_VGA;

	pt1_0.x = pt1.x / retiofx;  pt1_0.y = pt1.y / retiofy;
	pt2_0.x = pt2.x / retiofx;  pt2_0.y = pt2.y / retiofy;
	pt3_0.x = pt3.x / retiofx;  pt3_0.y = pt3.y / retiofy;
	pt4_0.x = pt4.x / retiofx;  pt4_0.y = pt4.y / retiofy;
	pt_center_0.x = pt_center.x / retiofx;  pt_center_0.y = pt_center.y / retiofy;


	HDC hdc;
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;

	hdc = GetDC(hWnd);
	hPen = CreatePen(PS_DOT, 1, RGB(0, 255, 0));
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	MoveToEx(hdc, pt1_0.x, pt1_0.y, NULL);
	LineTo(hdc, pt1_0.x, pt1_0.y);
	LineTo(hdc, pt2_0.x, pt2_0.y);
	LineTo(hdc, pt3_0.x, pt3_0.y);
	LineTo(hdc, pt4_0.x, pt4_0.y);
	Ellipse(hdc, pt_center.x - 2, pt_center.y - 2, pt_center.x + 2, pt_center.y + 2);

	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);

	ReleaseDC(hWnd, hdc);

	DeleteObject(hPen);
	DeleteObject(hBrush);

	return;
}



std::string CHelper::changeExt(const std::string path, const std::string ext) {
	std::string dstPath;

	//拡張子の位置を取得
	size_t pos = path.rfind('.');
	if (pos != std::string::npos) {
		std::string filename = path.substr(0, pos);
		//拡張子を入れ替える
		dstPath = filename + ext;
	}

	return dstPath;
};

RECT CHelper::mat2dispRoi(Rect matroi, cv::Size matsize) {
	RECT dispRoi, mdi_roi;

	float retiofx = float(matsize.width) / WIDTH_VGA;
	float retiofy = float(matsize.height) / HEIGHT_VGA;

	dispRoi.left = (LONG)(matroi.x / retiofx);
	dispRoi.top = (LONG)(matroi.y / retiofy) + DISP_OFFSET_Y;
	dispRoi.right = (LONG)((matroi.x + matroi.width) / retiofx);
	dispRoi.bottom = (LONG)((matroi.y + matroi.height) / retiofy) + DISP_OFFSET_Y;

	return dispRoi;
}
#endif