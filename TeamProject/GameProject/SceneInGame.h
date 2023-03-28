#pragma once
#include "Scene.h"
#include "CameraDebug.h"
#include "CameraTPS.h"
#include "DebugBox.h"
#include "FQuadTree.h"
#include "CharacterStateManager.h"
#include "EnemyNPCMob.h"
#include "Interface.h"

class SceneInGame : public Scene
{
public:
    virtual bool    Init() override;
    virtual bool    Frame() override;
    virtual bool    Render() override;
    virtual bool    Release() override;

    virtual void DataLoad() override;
    void    UiLoad();
    void    CameraLoad();
    void    FSMLoad();
    void    CharacterLoad();
    void    MapLoad();

private:
    bool		ObjectInit();
    bool		ObjectFrame();
    bool		ObjectRender();
    bool		ObjectRelease();

    E_SCENE NextScene();

public:
    Camera* m_pMainCamera = nullptr;

    bool m_Win = false;
    bool m_Defeat = false;

    int m_EnemyCount = 3;
    std::vector<SSB::EnemyNPCMob*> m_Enemies;
    //T_BOX       testBox;
    Interface* m_pInter;

    SSB::EnemyNPCMob* m_pEnemy = nullptr;
    T_BOX       testBox;

    DebugBox* m_pDebugBox = nullptr;
    std::vector<T_BOX*> m_debugBoxList;

    FQuadTree* m_pQuadTree = nullptr;

    std::map<SSB::StateManagerName, SSB::CharacterStateManager*> m_StateManagerMap;
};

