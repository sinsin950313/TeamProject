#pragma once
//#include <winsock2.h>
#include <windows.h>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <list>
#include <atlconv.h>
#include <codecvt>
#include <tchar.h>
#include <thread>
#include <random>
#include <wrl.h> // windows runtime c++ templace library(wrl)
#include "TMath.h"

#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "Ws2_32.lib")

using namespace Microsoft::WRL;
#define DXCORE

extern HWND		g_hWnd;
extern RECT		g_rcClient;
extern RECT		g_rcWindow;
extern UINT		g_iClientWidth;
extern UINT		g_iClientHeight;

extern float g_fGameTimer;
extern float g_fSecondPerFrame;


#define GAME_START int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){
#define GAME_WINDOW(c,s,x,y) c demo;demo.SetWindows(hInstance,L#s,x,y);demo.Run();return 1;
#define GAME_END    }
#define GAME_RUN(c,s,x,y)  GAME_START  GAME_WINDOW(c,s,x,y)  GAME_END


#define randf(x) (x*rand()/(float)RAND_MAX)
#define randf2(x,off) (off+x*rand()/(float)RAND_MAX)
#define randstep(fMin,fMax) (fMin+((float)fMax-(float)fMin)*rand()/(float)RAND_MAX)
#define clamp(x,MinX,MaxX) if (x>MaxX) x=MaxX; else if (x<MinX) x=MinX;


template<class T> class Singleton
{
public:
	static T& GetInstance()
	{
		static T instance;
		return instance;
	}
};

static double RandomStep(double min, double max = 1.0)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<>	 dist(min, max);
	return dist(gen);
};

//#include <atlconv.h> // A2W
static std::wstring to_mw(const std::string& _src)
{
	USES_CONVERSION;
	return std::wstring(A2W(_src.c_str()));
};

static std::string to_wm(const std::wstring& _src)
{
	USES_CONVERSION;
	return std::string(W2A(_src.c_str()));
};
static std::wstring mtw(std::string str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(str);
}
//#include <codecvt>
static std::string wtm(std::wstring str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.to_bytes(str);
}
static char* GetWtM(WCHAR* data)
{
	char retData[4096] = { 0 };
	// 변형되는 문자열의 크기가 반환된다.
	int iLength = WideCharToMultiByte(CP_ACP, 0,
		data, -1, 0, 0, NULL, NULL);
	int iRet = WideCharToMultiByte(CP_ACP, 0,
		data, -1,  //  소스
		retData, iLength, // 대상
		NULL, NULL);
	return retData;
}
static bool GetWtM(WCHAR* src, char* pDest)
{
	// 변형되는 문자열의 크기가 반환된다.
	int iLength = WideCharToMultiByte(CP_ACP, 0,
		src, -1, 0, 0, NULL, NULL);
	int iRet = WideCharToMultiByte(CP_ACP, 0,
		src, -1,  //  소스
		pDest, iLength, // 대상
		NULL, NULL);
	if (iRet == 0) return false;
	return true;
}
static WCHAR* GetMtW(char* data)
{
	WCHAR retData[4096] = { 0 };
	// 변형되는 문자열의 크기가 반환된다.
	int iLength = MultiByteToWideChar(CP_ACP, 0, data, -1, 0, 0);
	int iRet = MultiByteToWideChar(CP_ACP, 0,
		data, -1,  //  소스
		retData, iLength); // 대상
	return retData;
}
static bool GetMtW(char* pSrc, WCHAR* pDest)
{
	// 변형되는 문자열의 크기가 반환된다.
	int iLength = MultiByteToWideChar(CP_ACP, 0,
		pSrc, -1, 0, 0);
	int iRet = MultiByteToWideChar(CP_ACP, 0,
		pSrc, -1,  //  소스
		pDest, iLength); // 대상		
	if (iRet == 0) return false;
	return true;
}
static void PRINT(char* fmt, ...) // 나열연산자
{
	va_list arg;
	va_start(arg, fmt);
	char buf[256] = { 0, };
	vsprintf_s(buf, fmt, arg);
	printf("\n=====> %s", buf);
	va_end(arg);
}