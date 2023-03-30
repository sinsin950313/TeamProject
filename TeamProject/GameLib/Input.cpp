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
	GetWindowRect(g_hWnd, &g_rcWindow);

	POINT pt;
	pt.x = (g_rcWindow.right + g_rcWindow.left) / 2 + g_rcWindow.left;
	pt.y = (g_rcWindow.bottom + g_rcWindow.top) / 2 + g_rcWindow.top + 40;
	ScreenToClient(g_hWnd, &pt);
	GetCursorPos(&m_ptPos);

	if (m_isMouse)
	{
		m_ptOffset.x = m_ptPos.x - m_ptPrePos.x;
		m_ptOffset.y = m_ptPos.y - m_ptPrePos.y;
	}
	else
	{
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

void	Input::SwitchShowMouse(bool bMouse)
{
	m_isMouse = bMouse;
	ShowCursor(bMouse);
}

bool	Input::GetShowMouse()
{
	return m_isMouse;
}