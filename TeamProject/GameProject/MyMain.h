#pragma once
#include "GameCore.h"
#include "CameraDebug.h"
#include "CameraTPS.h"
#include "DebugBox.h"
#include "FQuadTree.h"
#include "CharacterStateManager.h"
#include <map>
#include "EnemyNPCMob.h"
#include "Interface.h"

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

    Interface* m_pInter;

    SSB::EnemyNPCMob* m_pEnemy = nullptr;
    T_BOX       testBox;

    DebugBox* m_pDebugBox = nullptr;
    std::vector<T_BOX*> m_debugBoxList;

    FQuadTree* m_pQuadTree = nullptr;

    std::map<SSB::StateManagerName, SSB::CharacterStateManager*> m_StateManagerMap;
};

