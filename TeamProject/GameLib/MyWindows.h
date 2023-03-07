#pragma once
#include "std.h"
class MyWindows
{
public:
	HINSTANCE		m_hInstance;
	DWORD			m_csStyle;

public:
	ATOM					MyRegisterClass();
	BOOL					InitInstance(const WCHAR* szTitle, UINT width, UINT height);
	virtual LRESULT		MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual HRESULT		ResizeDevice(UINT width, UINT height);

public:
	virtual int		WndProc(HWND, UINT, WPARAM, LPARAM);
public:
	void		SethWnd(HWND hWnd);
	bool		SetWindows(HINSTANCE hInstance, const WCHAR* szTitle, UINT iWidth = 1024, UINT iHeight = 768);
	void		CenterWindow();
	virtual bool		Run();

public:
	MyWindows();
};

