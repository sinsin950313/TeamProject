#pragma once
#include "std.h"

enum KEYSTATE
{
	KEY_FREE = 0,
	KEY_UP,
	KEY_PUSH,
	KEY_HOLD
};

class Input : public Singleton<Input>
{
private:
	friend class Singleton<Input>;
	Input() {};

private:
	DWORD m_dwKeyState[256];

public:
	POINT	m_ptPos;
	POINT	m_ptPrePos;
	POINT	m_ptOffset;

private:
	bool		m_isMouse = true;

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	DWORD GetKey(DWORD key);
	void	SwitchShowMouse(bool bMouse);
	bool	GetShowMouse();
};

#define I_Input Input::GetInstance()