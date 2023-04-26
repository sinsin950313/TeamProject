#pragma once
#include "Scene.h"
#include "CameraDebug.h"
#include "CameraTPS.h"
#include "CameraCinema.h"
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
    void    ScenarioLoad();
    
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
    Interface* m_pInter_Frame;
    Interface* m_pInter_Profile;
    Interface* m_pInter_MinimapFrame;
    Interface* m_pInter_PlayerHP;
    Interface* m_pInter_BossHP;
    Interface* m_pInter_Passive;
    Interface* m_pInter_Skill_Q;
    Interface* m_pInter_Skill_W;
    Interface* m_pInter_Skill_E;
    Interface* m_pInter_Skill_R;

    Interface* m_pInter_Damage_blood;

    Interface* m_pInter_GameTitle;
    Interface* m_pInter_PressStart;

    Interface* m_pInter_Win1;
    Interface* m_pInter_Win2;
    Interface* m_pInterFade;
    Interface* m_pInter_Defeat;
    Interface* m_pInterText;

    Camera* m_pMinimapCamera = nullptr;
    RenderTarget m_RenderTargetMinimap;

    Shader* m_pMinimapPS_Quadtree;
    Shader* m_pMinimapPS_Object;
    Shader* m_pMinimapPS_Skydome;
    Shader* m_pMinimapPS_DebugBox;

    Interface* m_pInter_Minimap;
    InterfaceMinimap* m_pInter_MinimapContents = nullptr;
    InterfaceMinimap* m_pInter_Minimap_player = nullptr;

    CameraCinema* m_pCinemaCamera = nullptr;
    Camera* m_pCameraCurrent = nullptr;

    void SetCinemaCamera(std::wstring szCinemaName);
    void SetMainCamera();
    void MoveCinemaCamera();

    UINT m_iCurrentCineCount = 0;
    bool m_bIngame1_CinemaIntro_Start = false;
    bool m_bIngame1_CinemaIntro_End = false;
    bool m_bIngame1_houdgiend = false;
    bool m_bIngame1_Cinema1_Start = false;
    bool m_bIngame1_Cinema1_End = false;
    bool m_bIngame1_Cinema2_Start = false;
    bool m_bIngame1_Cinema2_End = false;
    bool m_bIngame1_EnemyHalfCount = false;

    bool m_bIngame2_CinemaIntro_Start = false;
    bool m_bIngame2_CinemaIntro_End = false;
    bool m_bIngame2_CinemaOver = false;
    bool m_bIngame2_EnemyHalfCount = false;
    std::map<std::wstring, std::wstring> m_ScenarioList;

    TVector3 m_vBossSpawnPos;

public:
    Sound* sound_bgm;
    Sound* sound_houdgi;
  
public:

    virtual ~SceneInGame();

};

