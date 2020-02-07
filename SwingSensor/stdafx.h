// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#pragma comment(lib, "winmm.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>

// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: プログラムに必要な追加ヘッダーをここで参照してください
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <pylon/PylonIncludes.h>
#include <pylon/PylonGUI.h>
#include <pylon/BaslerUniversalInstantCamera.h>

using namespace std;
using namespace cv;
using namespace GenApi;
using namespace Pylon;

#include "CodeDef.h"

//iostream用
#include <iostream>
#include <iomanip>
#include <sstream> //stringstream用
#include <vector>

//マルチメディアタイマー用
#include <mmsystem.h>
