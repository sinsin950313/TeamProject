#pragma once
#include "GameCore.h"
#include "CCameraDebug.h"
#include "ModelTest.h"

class MyMain :
    public GameCore
{
public:
    bool    Init();
    bool    Frame();
    bool    Render();
    bool    Release();

public:
    CCamera* m_pMainCamera = nullptr;

    ModelTest* m_pModelTest = nullptr;
};

