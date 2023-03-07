#pragma once
#include "GameCore.h"
#include "CCameraDebug.h"
#include "Model.h"

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
    SSB::Model* m_pModel = nullptr;
};

