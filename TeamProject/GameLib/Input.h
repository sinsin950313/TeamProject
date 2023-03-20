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

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	DWORD GetKey(DWORD key);
};

#define I_Input Input::GetInstance()