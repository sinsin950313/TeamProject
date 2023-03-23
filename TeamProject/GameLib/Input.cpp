#include "Input.h"


bool	Input::Init()
{
	ZeroMemory(&m_dwKeyState, sizeof(m_dwKeyState));
	::GetCursorPos(&m_ptPos); // 화면좌표
	::ScreenToClient(g_hWnd, &m_ptPos); // 클라이언트
	m_ptPrePos = m_ptPos;
	return true;
}

bool	Input::Frame()
{
	POINT pt;
	pt.x = (g_rcClient.right - g_rcClient.left) / 2;
	pt.y = (g_rcClient.bottom - g_rcClient.top) / 2;

	ShowCursor(m_isMouse);
	if (m_isMouse)
	{
		GetCursorPos(&m_ptPos);
		ScreenToClient(g_hWnd, &m_ptPos);
		m_ptOffset.x = m_ptPos.x - m_ptPrePos.x;
		m_ptOffset.y = m_ptPos.y - m_ptPrePos.y;
	}
	else
	{
		GetCursorPos(&m_ptPos);
		//ScreenToClient(g_hWnd, &m_ptPos);
		m_ptOffset.x = m_ptPos.x - pt.x;// m_ptPrePos.x;
		m_ptOffset.y = m_ptPos.y - pt.y;// m_ptPrePos.y;
	}
	

	for (int i = 0; i < 256; i++)
	{
		SHORT sKey = ::GetAsyncKeyState(i);
		if (sKey & 0x8000)
		{
			if (m_dwKeyState[i] == KEY_UP ||
				m_dwKeyState[i] == KEY_FREE)
				m_dwKeyState[i] = KEY_PUSH;
			else
				m_dwKeyState[i] = KEY_HOLD;
		}
		else
		{
			if (m_dwKeyState[i] == KEY_PUSH ||
				m_dwKeyState[i] == KEY_HOLD)
				m_dwKeyState[i] = KEY_UP;
			else
				m_dwKeyState[i] = KEY_FREE;
		}
	}
	m_ptPrePos = m_ptPos;

	if(!m_isMouse)
		SetCursorPos(pt.x, pt.y);
	return true;
}

bool	Input::Render()
{
	std::wstring str = std::to_wstring(m_ptPos.x) + L" " + std::to_wstring(m_ptPos.y) + L'\n';
	OutputDebugString(str.c_str());
	return true;
}

bool	Input::Release()
{
	return true;
}

DWORD Input::GetKey(DWORD key)
{
	return m_dwKeyState[key];
}