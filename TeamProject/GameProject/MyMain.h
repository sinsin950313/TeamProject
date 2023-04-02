#pragma once

#include "GameCore.h"
#include "MultiRenderTarget.h"
#include "Screen.h"

class MyMain :
    public GameCore
{
public:
    bool    Init();
    bool    Frame();
    bool    Render();
    bool    Release();

public:
    virtual int		WndProc(HWND, UINT, WPARAM, LPARAM);

public:
    SSB::MultiRenderTarget m_MRT;
    SSB::Screen m_screen;
};

