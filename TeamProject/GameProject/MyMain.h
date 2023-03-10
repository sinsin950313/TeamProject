#pragma once
#include "GameCore.h"
#include "CCameraDebug.h"
#include "ModelTest.h"
#include "DebugBox.h"

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
    CCamera* m_pMainCamera = nullptr;

    ModelTest* m_pModelTest = nullptr;
    DebugBox* m_pDebugBox = nullptr;
};

