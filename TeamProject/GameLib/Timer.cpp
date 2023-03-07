#include "Timer.h"

float	g_fGameTimer = 0.0f;
float	g_fSecondPerFrame = 0.0f;

bool Timer::Init()
{
	m_dwBeforeTime = timeGetTime();
	QueryPerformanceFrequency(&m_Frequency);
	QueryPerformanceCounter(&m_Frame);
	return true;
}

bool Timer::Frame()
{
	QueryPerformanceCounter(&m_Current);
	m_fElapseTimer = (float)(m_Current.QuadPart - m_Frame.QuadPart) / (float)m_Frequency.QuadPart;

	/*
	DWORD dwCurTime = timeGetTime();
	DWORD dwElapseTime = dwCurTime - m_dwBeforeTime;
	m_fElapseTimer = dwElapseTime / 1000.0f;
	*/
	m_fGameTimer += m_fElapseTimer;

	m_iFPSCounter++;
	m_fFPSTimer += m_fElapseTimer;
	if (m_fFPSTimer >= 1.0f)
	{
		m_iFPS = m_iFPSCounter;
		m_iFPSCounter = 0;
		m_fFPSTimer -= 1.0f;
	}

	g_fGameTimer = m_fGameTimer;
	g_fSecondPerFrame = m_fElapseTimer;
	//m_dwBeforeTime = dwCurTime;

	m_Frame = m_Current;
	return true;
}

bool Timer::Render()
{
	m_szTimer = std::to_wstring(m_fGameTimer) + L" " + std::to_wstring(m_iFPS) + L'\n';
	OutputDebugString(m_szTimer.c_str());
	return true;
}

bool Timer::Release()
{
	return true;
}
