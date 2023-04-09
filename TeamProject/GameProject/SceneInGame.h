#pragma once
#include "Scene.h"
#include "CameraDebug.h"
#include "CameraTPS.h"
#include "DebugBox.h"
#include "FQuadTree.h"
#include "CharacterStateManager.h"
#include "EnemyNPCMob.h"
#include "BossMob.h"

class SceneInGame : public Scene
{
public:
    virtual void DataLoad() override;
    void    UiLoad();
    void    CameraLoad();
    void    FSMLoad();
    void    CharacterLoad();
    void    MapLoad();
    virtual bool    Init();
    virtual bool    Frame();
    virtual bool PreRender() override;
    virtual bool    Render();
    virtual bool    Release();

private:
    E_SCENE NextScene();

public:
    Camera* m_pMainCamera = nullptr;

    bool m_Win = false;
    bool m_Defeat = false;

    std::vector<SSB::EnemyNPCMob*> m_Enemies;
    //T_BOX       testBox;
    Interface* m_pInter;

    SSB::EnemyNPCMob* m_pEnemy = nullptr;
    T_BOX       testBox;

    DebugBox* m_pDebugBox = nullptr;
    std::vector<T_BOX*> m_debugBoxList;

    FQuadTree* m_pQuadTree = nullptr;

    std::map<SSB::StateManagerName, SSB::CharacterStateManager*> m_StateManagerMap;

    SSB::BossMob* m_pBoss = nullptr;
};

