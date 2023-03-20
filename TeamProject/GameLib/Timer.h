#pragma once
#include "std.h"
#pragma comment(lib, "winmm.lib") // timeGetTime
class Timer : public Singleton<Timer>
{
private:
	friend class Singleton<Timer>;
	
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();

public:
	LARGE_INTEGER m_Frequency;
	LARGE_INTEGER m_Current;
	LARGE_INTEGER m_Frame;

public:
	float	m_fGameTimer = 0.0f;
	float	m_fElapseTimer = 0.0f;
	UINT	m_iFPS = 0;
	std::wstring m_szTimer;

private:
	DWORD m_dwBeforeTime;
	UINT	m_iFPSCounter = 0;
	float	m_fFPSTimer = 0.0f;
};

#define I_Timer Timer::GetInstance()
