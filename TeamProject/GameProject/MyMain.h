#pragma once
#include "GameCore.h"
#include "CameraDebug.h"
#include "CameraTPS.h"
#include "Character.h"
#include "DebugBox.h"
#include "FQuadTree.h"
#include "CharacterStateManager.h"
#include <map>

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
    T_BOX       testBox;

    DebugBox* m_pDebugBox = nullptr;
    std::vector<T_BOX*> m_debugBoxList;

    FQuadTree* m_pQuadTree = nullptr;

    std::map<SSB::StateManagerName, SSB::CharacterStateManager*> m_StateManagerMap;
};

