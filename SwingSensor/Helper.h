#pragma once
#include "stdafx.h"

//  共有メモリステータス
#define	OK_SHMEM			0			// 共有メモリ 生成/破棄正常
#define	ERR_SHMEM_CREATE	-1			// 共有メモリ Create異常
#define	ERR_SHMEM_VIEW		-2			// 共有メモリ View異常

#define	ON				1		// ON
#define	OFF				0		// OFF

#define TX_DRAW_ROW_INTERVAL  28
#define TX_DRAW_ROW_MAX  20
#define WIDTH_VGA 640
#define HEIGHT_VGA 480
#define DISP_OFFSET_Y 30
#define DRAW_LINE_MOUSE_PT  0x0000001
#define DRAW_LINE_DISP_ROI  0x0000002

class CHelper
{
public:
	CHelper();
	~CHelper();
	static void Str2Wstr(const std::string &src, std::wstring &dest);
	static void put_bmp_built(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt);
	static void put_bmp_stretch(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt, int retio_persent);
	static int	cmnCreateShmem(LPCTSTR, DWORD, HANDLE*, LPVOID*, DWORD*);	//共有メモリ設定
	static int	cmnDeleteShMem(HANDLE*, LPVOID*);							//共有メモリ解除
	static int create_file_path_of_exe_folder(LPWSTR pst_file_path, const wchar_t* filename, const wchar_t* ext);
	int dispbmp(HWND hWnd, BITMAPINFO bmpInfo, PUINT8 lpPixcel, int com, POINT pt, RECT rio_rect, UINT32 color, float retiofx, float retiofy);
	int dispmat(Mat s_mat, HWND hWnd);
	int put_line(BITMAPINFO bmpIf, POINT pt, PUINT8 lpPixcel, UINT32 color, float retiofx, float retiofy);
	POINT get_disp_size() { POINT pt; pt.x = WIDTH_VGA;  pt.y = HEIGHT_VGA; return pt; }
	int put_rect(BITMAPINFO bmpIf, RECT rect, PUINT8 lpPixcel, UINT32 color, float retiofx, float retiofy);
	int put_rect(HWND hWnd, RECT rect, UINT32 color);
	void DrawRect(HWND, POINTS, POINTS);
	void DrawRect(HWND hWnd, Point2f pt1, Point2f pt2, Point2f pt3, Point2f pt4, Point2f center, cv::Size matsize);
	std::string changeExt(const std::string path, const std::string ext);
	RECT mat2dispRoi(Rect matroi, cv::Size matsize);
};
