#include "SceneInGame.h"
#include "Input.h"
#include "SoundMgr.h"
#include "FileIOObject.h"
#include "Player.h"
#include "TCollision.h"
#include "CollisionMgr.h"
#include "ModelMgr.h"
#include "CharacterStateManager.h"
#include "PlayerStateService.h"
#include "EnemyNPCMobStateService.h"
#include "CommonPath.h"
#include "DirectionalLight.h"
#include "LightManager.h"

E_SCENE SceneInGame::NextScene()
{
    //if (m_pPlayer->m_iLife < 0)
    //{
    //    return S_END;
    //}
    //if (I_Enemy.m_pBoss)
    //{
    //    if (I_Enemy.m_pBoss->m_iLife == 0)
    //    {
    //        return S_END;
    //    }
    //}
    return S_INGAME;
}

bool    SceneInGame::Init()
{
    SSB::DirectionalLight* light = new SSB::DirectionalLight;
    light->Initialize_SetDevice(m_pd3dDevice, m_pImmediateContext);
    light->Init();
    light->SetLightDirection(TVector3(0, -1, 1));
    SSB::I_Light.GetInstance().SetLight(light);

    I_Model.SetDevice(m_pd3dDevice, m_pImmediateContext);

    I_Sound.LoadDir(kTeamProjectSoundPath);
    I_Sound.LoadAll(kTeamProjectSoundPath);

    m_pInter = new Interface();
    m_pInter->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/ui.png");
    m_pInter->m_vPos = TVector3(0, 0, 0);
    m_pInter->m_vScale = TVector3(1, 1, 1);
    //m_pInter->m_pWorkList.push_back(new InterfaceFade());
    //m_pInter->m_pWorkList.push_back(new InterfaceLoopFade(1.0f));
    //m_pInter->m_pWorkList.push_back(new InterfaceLifeTime(10.0f));
    //m_pInter->m_pWorkList.push_back(new InterfaceClick(m_pInter->m_vScale.x));

    m_pMainCamera = new CameraTPS;
    m_pMainCamera->CreateViewMatrix(TVector3(0, 0, -30), TVector3(0, 0, 0.1f), TVector3(0, 1, 0));
    m_pMainCamera->CreateProjMatrix(0.1f, 1500.0f, XM_PI * 0.25f
        , (float)g_rcClient.right / (float)g_rcClient.bottom);

    // Register State Manager
    {
        {
            SSB::CharacterStateManager* manager = new SSB::CharacterStateManager;

            {
                SSB::CharacterState* state = new SSB::PlayerIdleState;
                state->Initialize_SetCoolTime(0);
                state->Initialize_SetStateAnimation("Idle");
                manager->Initialize_RegisterState(SSB::kPlayerIdle, state);
            }
            {
                SSB::CharacterState* state = new SSB::PlayerMoveState;
                state->Initialize_SetCoolTime(0);
                state->Initialize_SetStateAnimation("Move");
                state->Initialize_SetEffectSound(I_Sound.Find(L"GarenWalk.mp3"), true);
                manager->Initialize_RegisterState(SSB::kPlayerMove, state);
            }
            {
                SSB::CharacterState* state = new SSB::PlayerAttackState1;
                state->Initialize_SetCoolTime(1.8f);
                state->Initialize_SetStateAnimation("Attack1");
                state->Initialize_SetEffectSound(I_Sound.Find(L"GarenAttack1.mp3"));
                manager->Initialize_RegisterState(SSB::kPlayerAttack1, state);
            }
            {
                SSB::CharacterState* state = new SSB::PlayerAttackState2;
                state->Initialize_SetCoolTime(1.8f);
                state->Initialize_SetStateAnimation("Attack2");
                state->Initialize_SetEffectSound(I_Sound.Find(L"GarenAttack2.mp3"));
                manager->Initialize_RegisterState(SSB::kPlayerAttack2, state);
            }
            {
                SSB::CharacterState* state = new SSB::PlayerAttackState3;
                state->Initialize_SetCoolTime(1.8f);
                state->Initialize_SetStateAnimation("Attack3");
                state->Initialize_SetEffectSound(I_Sound.Find(L"GarenAttack3.mp3"));
                manager->Initialize_RegisterState(SSB::kPlayerAttack3, state);
            }
            {
                SSB::CharacterState* state = new SSB::PlayerDeadState;
                state->Initialize_SetCoolTime(0);
                state->Initialize_SetStateAnimation("Dead");
                state->Initialize_SetEffectSound(I_Sound.Find(L"GarenDead.mp3"));
                manager->Initialize_RegisterState(SSB::kPlayerDead, state);
            }

            m_StateManagerMap.insert(std::make_pair(SSB::kPlayerStateManager, manager));
        }

        {
            SSB::CharacterStateManager* manager = new SSB::CharacterStateManager;
            {
                SSB::CharacterState* state = new SSB::EnemyNPCMobIdleState;
                state->Initialize_SetCoolTime(0);
                state->Initialize_SetStateAnimation("Idle");
                manager->Initialize_RegisterState(SSB::kEnemyNPCMobIdle, state);
            }
            {
                SSB::CharacterState* state = new SSB::EnemyNPCMobMoveState;
                state->Initialize_SetCoolTime(0);
                state->Initialize_SetStateAnimation("Move");
                state->Initialize_SetEffectSound(I_Sound.Find(L"AlistarWalk.mp3"), true);
                manager->Initialize_RegisterState(SSB::kEnemyNPCMobMove, state);
            }
            {
                SSB::CharacterState* state = new SSB::EnemyNPCMobAttackState;
                state->Initialize_SetCoolTime(1.5f);
                state->Initialize_SetStateAnimation("Attack1");
                state->Initialize_SetEffectSound(I_Sound.Find(L"AlistarAttack1.mp3"));
                manager->Initialize_RegisterState(SSB::kEnemyNPCMobAttack, state);
            }
            {
                SSB::CharacterState* state = new SSB::EnemyNPCMobDeadState;
                state->Initialize_SetCoolTime(0);
                state->Initialize_SetStateAnimation("Dead");
                state->Initialize_SetEffectSound(I_Sound.Find(L"AlistarDead.mp3"));
                manager->Initialize_RegisterState(SSB::kEnemyNPCMobDead, state);
            }

            m_StateManagerMap.insert(std::make_pair(SSB::kEnemyNPCMobStateManager, manager));
        }
    }

    {
        Player::GetInstance().SetDevice(m_pd3dDevice, m_pImmediateContext);
        Player::GetInstance().m_pMainCamera = m_pMainCamera;
        ((CameraTPS*)m_pMainCamera)->m_vFollowPos = &Player::GetInstance().m_vPos;

        //Idle, Attack1, Attack2, Attack3, Move, Dead
        //I_Model.Load(filename, str, "Idle", &Player::GetInstance().m_pModel);
        I_Model.Load("Yasuo", "Idle", &Player::GetInstance().m_pModel);

        Player::GetInstance().Initialize_SetPosition(TVector3(0, 0, 0));
        Player::GetInstance()._damagedSound = I_Sound.Find(L"GarenDamaged.mp3");
        Player::GetInstance().m_Damage = 100;
        Player::GetInstance().Init();
        Player::GetInstance().Scale(0.01f);

        m_StateManagerMap.find(SSB::kPlayerStateManager)->second->RegisterCharacter(&Player::GetInstance(), SSB::kPlayerIdle);
    }

    {
        for (int i = 0; i < m_EnemyCount; ++i)
        {
            SSB::EnemyNPCMob* enemy = new SSB::EnemyNPCMob();
            enemy->SetDevice(m_pd3dDevice, m_pImmediateContext);
            I_Model.Load("Alistar", "Idle", &enemy->m_pModel);

            enemy->Initialize_SetPosition(TVector3(-50 + i * 50, 0, -50));
            enemy->m_Damage = 10;
            enemy->m_fSpeed = 10;
            enemy->_damagedSound = I_Sound.Find(L"AlistarDamaged.mp3");
            enemy->Init();
            enemy->Scale(0.01f);
            /*
                    std::string filename = "dummy";
                    std::string str = io.Read(filename);

                    m_pEnemy = new SSB::EnemyNPCMob();
                    m_pEnemy->SetDevice(m_pd3dDevice, m_pImmediateContext);
                    I_Model.Load(filename, str, "Idle", &m_pEnemy->m_pModel);
            */

            m_StateManagerMap.find(SSB::kEnemyNPCMobStateManager)->second->RegisterCharacter(enemy, SSB::kEnemyNPCMobIdle);

            m_Enemies.push_back(enemy);
        }
    }

    //m_debugBoxList.push_back(&Player::GetInstance().m_ColliderBox);
    //m_debugBoxList.push_back(&Player::GetInstance().m_AttackBox);

    //m_debugBoxList.push_back(&m_pEnemy->m_ColliderBox);
    //m_debugBoxList.push_back(&m_pEnemy->m_AttackBox);

    //testBox.CreateOBBBox(40, 4, 4);
    //m_debugBoxList.push_back(&testBox);
    //I_Collision.AddStaticObjectBox(&testBox, NULL);

    m_pDebugBox = new DebugBox;
    m_pDebugBox->Create(m_pd3dDevice, m_pImmediateContext);

    m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/temp_8_11_5_2.map", m_pd3dDevice, m_pImmediateContext);
    //m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/temp_8_8.map", m_pd3dDevice, m_pImmediateContext);
    m_pQuadTree->m_pCurrentCamera = m_pMainCamera;


    Sound* sound = I_Sound.Find(L"BGM.mp3");
    sound->Play(true);
    return true;
}

bool    SceneInGame::Frame()
{
    //if (m_Win)
    //{
    //    if (MessageBoxA(g_hWnd, "You Win!", "Win!", MB_OK))
    //    {
    //        m_bGameRun = false;
    //    }
    //}
    //else if (m_Defeat)
    //{
    //    if (MessageBoxA(g_hWnd, "You Lose!", "Defeat!", MB_OK))
    //    {
    //        m_bGameRun = false;
    //    }
    //}

    
    if (I_Input.GetKey(VK_F3) == KEY_PUSH)
        I_Input.m_isMouse = !I_Input.m_isMouse;

    for (auto manager : m_StateManagerMap)
    {
        manager.second->Frame();
    }

    if (m_pMainCamera)
    {
        m_pMainCamera->Frame();

        int deadCount = 0;
        for (auto enemy : m_Enemies)
        {
            if (enemy->IsDead())
            {
                ++deadCount;
            }
        }
        if (deadCount == m_Enemies.size())
        {
            m_Win = true;
        }

        if (Player::GetInstance().IsDead())
        {
            m_Defeat = true;
        }
    }

    Player::GetInstance().Frame();
    m_pQuadTree->Update();

    for (auto enemy : m_Enemies)
    {
        enemy->Frame();
    }

    //m_pEnemy->Frame();
    m_pInter->Frame();
    //modelBox.UpdateBox(Player::GetInstance().m_matWorld);
    return true;
}

//bool SceneInGame::PreRender()
//{
//    m_pQuadTree->SetMatrix(nullptr, &light->m_matView, &light->m_matProj);
//    m_pQuadTree->PreRender();
//
//    Player::GetInstance().SetMatrix(nullptr, &light->m_matView, &light->m_matProj);
//    Player::GetInstance().PreRender();
//
//    for (auto enemy : m_Enemies)
//    {
//        enemy->SetMatrix(nullptr, &light->m_matView, &light->m_matProj);
//        enemy->PreRender();
//    }
//
//    return true;
//}

bool    SceneInGame::Render()
{
    //m_pQuadTree->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    //m_pQuadTree->Render();

    Player::GetInstance().SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    Player::GetInstance().Render();

    for (auto enemy : m_Enemies)
    {
        enemy->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
        enemy->Render();
    }

    if (m_pDebugBox)
    {
        //for (auto box : I_Collision.GetMapCollisionList())
        //{
        //    m_pDebugBox->SetMatrix(&m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
        //    m_pDebugBox->SetBox(box);
        //    m_pDebugBox->SetColor({1, 0, 0, 1});
        //    m_pDebugBox->UpdateBuffer();
        //    m_pDebugBox->Render();
        //}

        m_pDebugBox->SetMatrix(&m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
        TColor color = TColor(0, 0, 1, 1);
        for (T_BOX* box : m_debugBoxList)
        {
            m_pDebugBox->SetBox(*box);
            m_pDebugBox->SetColor(color);
            m_pDebugBox->UpdateBuffer();
            m_pDebugBox->Render();
        }

        T_BOX b;
        b.CreateOBBBox(0.2, 0.2, 0.2, Player::GetInstance().GetCurSocketPos("WeaponHigh"));
        m_pDebugBox->SetBox(b);
        m_pDebugBox->SetColor(TColor(1, 1, 1, 1));
        m_pDebugBox->UpdateBuffer();
        //m_pDebugBox->Render();

        TVector3 cen = (Player::GetInstance().GetCurSocketPos("WeaponLow") + b.vCenter) * 0.5;
        TVector3 l = (Player::GetInstance().GetCurSocketPos("WeaponLow") - cen);
        float ex = D3DXVec3Length(&l);

        TVector3 axis[3];
        axis[0] = -l;
        D3DXVec3Normalize(&axis[0], &axis[0]);

        TVector3 B;
        D3DXVec3Cross(&B, &axis[0], &TVector3::UnitX);
        if (B == TVector3::Zero)
        {
            D3DXVec3Cross(&B, &axis[0], &TVector3::UnitY);
        }
        TVector3 C;
        D3DXVec3Cross(&C, &axis[0], &B);

        b.CreateOBBBox(ex, 0.3, 0.3, cen, axis[0], B, C);
        m_pDebugBox->SetBox(b);
        m_pDebugBox->SetColor(TColor(1, 1, 1, 1));
        m_pDebugBox->UpdateBuffer();
        //m_pDebugBox->Render();

        //m_pDebugBox->SetBox(TVector3(0, 0, 0), TVector3::Zero, TVector3::One);
        //T_BOX box;
        //box.CreateOBBBox();
        //m_pDebugBox->Render();
    }

    //Player::GetInstance().m_pTrail->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    //Player::GetInstance().m_pTrail->Render();
    //m_pInter->Render();

	auto lights = SSB::I_Light.GetLightList();
	for (auto light : lights)
	{
		light->SetMatrix(&Player::GetInstance().m_matWorld, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
	}

    return true;
}

bool    SceneInGame::Release()
{
    if (m_pInter)
    {
        m_pInter->Release();
        delete m_pInter;
        m_pInter = nullptr;
    }

    if (m_pQuadTree)
    {
        m_pQuadTree->Release();
        delete m_pQuadTree;
        m_pQuadTree = nullptr;
    }

    if (m_pMainCamera)
    {
        m_pMainCamera->Release();
        delete m_pMainCamera;
    }

    for (auto enemy : m_Enemies)
    {
        enemy->Release();
        delete enemy;
    }

    if (m_pDebugBox)
    {
        m_pDebugBox->Release();
        delete m_pDebugBox;
    }

    for (auto manager : m_StateManagerMap)
    {
        manager.second->Release();
        delete manager.second;
    }
    m_StateManagerMap.clear();

    return true;
}


bool		SceneInGame::ObjectInit()
{
    return true;
}

bool		SceneInGame::ObjectFrame()
{
    return true;
}

bool		SceneInGame::ObjectRender()
{
    return true;
}

bool		SceneInGame::ObjectRelease()
{
    return true;
}