#pragma once
#include "GameCore.h"
#include "CameraDebug.h"
#include "CameraTPS.h"
#include "ModelTest.h"
#include "Character.h"
#include "DebugBox.h"
#include "FQuadTree.h"
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
    Camera* m_pMainCamera = nullptr;

    Character* m_pModelTest = nullptr;
    DebugBox* m_pDebugBox = nullptr;

    FQuadTree* m_pQuadTree = nullptr;
};

