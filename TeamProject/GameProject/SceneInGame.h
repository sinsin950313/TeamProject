#pragma once
#include "Scene.h"
#include "CameraDebug.h"
#include "CameraTPS.h"
#include "DebugBox.h"
#include "FQuadTree.h"
#include "CharacterStateManager.h"
#include "EnemyNPCMob.h"
#include "BossMob.h"
#include "RenderTarget.h"
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
    E_SCENE NextScene();

public:
    Camera* m_pMainCamera = nullptr;

    bool m_Win = false;
    bool m_Defeat = false;

    std::vector<SSB::EnemyNPCMob*> m_Enemies;
    //T_BOX       testBox;


    SSB::EnemyNPCMob* m_pEnemy = nullptr;
    T_BOX       testBox;

    DebugBox* m_pDebugBox = nullptr;
    std::vector<T_BOX*> m_debugBoxList;

    FQuadTree* m_pQuadTree = nullptr;

    std::map<SSB::StateManagerName, SSB::CharacterStateManager*> m_StateManagerMap;

    SSB::BossMob* m_pBoss = nullptr;

    Interface* m_pInter_Ingame;
    Interface* m_pInter_PlayerHP;
    Interface* m_pInter_Passive;
    Interface* m_pInter_Skill_Q;
    Interface* m_pInter_Skill_W;
    Interface* m_pInter_Skill_E;
    Interface* m_pInter_Skill_R;
    Interface* m_pMini;
    Interface* m_pInter_Minimap;
    RenderTarget m_Minimap;
    std::vector<DamageFont> m_DamageFontList;
};

