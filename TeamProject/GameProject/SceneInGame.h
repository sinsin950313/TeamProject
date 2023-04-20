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
    virtual void DataLoad() override;
    void    UiLoad();
    void    CameraLoad();
    void    FSMLoad();
    void    CharacterLoad();
    void    MapLoad();
    
    void    RenderMinimap();

    virtual bool    Init();
    virtual bool    Frame();
    virtual bool    PreRender() override;
    virtual bool    Render();
    virtual bool    PostRender() override;
    virtual bool    Release();

private:
    E_SCENE NextScene();

public:
    Camera* m_pMainCamera = nullptr;

    bool m_bInteractNextStage = false;
    bool m_Win = false;
    bool m_Defeat = false;
    
    //std::vector<SSB::EnemyNPCMob*> m_Enemies;
    std::vector<Character*> m_Enemies;
    UINT m_iMobDeadCount = 0;
    //T_BOX       testBox;


    //SSB::EnemyNPCMob* m_pEnemy = nullptr;
    T_BOX       testBox;

    DebugBox* m_pDebugBox = nullptr;
    std::vector<T_BOX*> m_debugBoxList;

    FQuadTree* m_pQuadTree = nullptr;

    std::map<SSB::StateManagerName, SSB::CharacterStateManager*> m_StateManagerMap;

    SSB::BossMob* m_pBoss = nullptr;

    std::vector<DamageFont> m_DamageFontList;
    Interface* m_pInter_Ingame;
    Interface* m_pInter_PlayerHP;
    Interface* m_pInter_BossHP;
    Interface* m_pInter_Passive;
    Interface* m_pInter_Skill_Q;
    Interface* m_pInter_Skill_W;
    Interface* m_pInter_Skill_E;
    Interface* m_pInter_Skill_R;

    Camera* m_pMinimapCamera = nullptr;
    RenderTarget m_RenderTargetMinimap;

    Shader* m_pMinimapPS_Quadtree;
    Shader* m_pMinimapPS_Object;
    Shader* m_pMinimapPS_Skydome;
    Shader* m_pMinimapPS_DebugBox;

    Interface* m_pInter_Minimap;
    InterfaceMinimap* m_pInter_MinimapContents = new InterfaceMinimap();
    InterfaceMinimap* m_pInter_Minimap_player = new InterfaceMinimap();

public:

    virtual ~SceneInGame();

};

