#include "MyMain.h"
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

int		MyMain::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (m_pMainCamera)
    {
        m_pMainCamera->WndProc(hWnd, msg, wParam, lParam);
    }
    return -1;
}

bool    MyMain::Init()
{
    I_Model.SetDevice(m_pd3dDevice, m_pImmediateContext);
    //I_Sound.LoadAll(L"");
    //Sound* pSound = I_Sound.Find(L"");
    //pSound->Play(true);

    m_pMainCamera = new CameraTPS;
    m_pMainCamera->CreateViewMatrix(TVector3(0, 0, -30), TVector3(0, 0, 0.1f), TVector3(0, 1, 0));
    m_pMainCamera->CreateProjMatrix(0.1f, 1500.0f, XM_PI * 0.25f
        , (float)g_rcClient.right / (float)g_rcClient.bottom);

    // Register State Manager
	{
        {
            SSB::CharacterStateManager* manager = new SSB::CharacterStateManager;
            manager->Initialize_RegisterState(SSB::kPlayerIdle, new SSB::PlayerIdleState);
            manager->Initialize_RegisterState(SSB::kPlayerMove, new SSB::PlayerMoveState);
            manager->Initialize_RegisterState(SSB::kPlayerAttack, new SSB::PlayerAttackState);

            m_StateManagerMap.insert(std::make_pair(SSB::kPlayerStateManager, manager));
        }

        {
            SSB::CharacterStateManager* manager = new SSB::CharacterStateManager;
            manager->Initialize_RegisterState(SSB::kEnemyNPCMobIdle, new SSB::EnemyNPCMobIdleState);
            manager->Initialize_RegisterState(SSB::kEnemyNPCMobMove, new SSB::EnemyNPCMobMoveState);
            manager->Initialize_RegisterState(SSB::kEnemyNPCMobAttack, new SSB::EnemyNPCMobAttackState);

			m_StateManagerMap.insert(std::make_pair(SSB::kEnemyNPCMobStateManager, manager));
        }
    }

    {
        SSB::ObjectScriptIO io;
        std::string str = io.Read("PlayerGaren");

        Player::GetInstance().SetDevice(m_pd3dDevice, m_pImmediateContext);
        Player::GetInstance().m_pMainCamera = m_pMainCamera;
        ((CameraTPS*)m_pMainCamera)->m_vFollowPos = &Player::GetInstance().m_vPos;

        //Idle, Attack1, Attack2, Attack3, Move, Dead
        I_Model.Load(str, "Idle", &Player::GetInstance().m_pModel);

        Player::GetInstance().Init();
        Player::GetInstance().Scale(0.01f);

        m_StateManagerMap.find(SSB::kPlayerStateManager)->second->RegisterCharacter(&Player::GetInstance(), SSB::kPlayerIdle);
    }

    {
        SSB::ObjectScriptIO io;
        std::string str = io.Read("Alistar");

        m_pEnemy = new SSB::EnemyNPCMob();
        m_pEnemy->SetDevice(m_pd3dDevice, m_pImmediateContext);
        I_Model.Load(str, "Idle", &m_pEnemy->m_pModel);

        //m_pEnemy->Initialize_SetPosition(TVector3(-100, 0, 0));
        m_pEnemy->Init();
        m_pEnemy->Scale(0.01f);

        m_StateManagerMap.find(SSB::kEnemyNPCMobStateManager)->second->RegisterCharacter(m_pEnemy, SSB::kEnemyNPCMobIdle);
    }

    //modelBox.CreateAABBBox(m_pModelTest->m_pModel->_maxVertex, m_pModelTest->m_pModel->_minVertex);
    //modelBox.CreateOBBBox(1, 2, 1);
    //m_debugBoxList.push_back(&modelBox);
    //m_debugBoxList.push_back(&Player::GetInstance().m_ColliderBox);
    //m_debugBoxList.push_back(&Player::GetInstance().m_AttackBox);
    //I_Collision.AddBox(&Player::GetInstance().m_AttackBox, &Player::GetInstance());

    //testBox.CreateOBBBox(40, 4, 4);
    //m_debugBoxList.push_back(&testBox);

    I_Collision.AddStaticObjectBox(&testBox, NULL);

    m_pDebugBox = new DebugBox;
    m_pDebugBox->Create(m_pd3dDevice, m_pImmediateContext);

    m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/temp_8_8.map", m_pd3dDevice, m_pImmediateContext);

    return true;
}

bool    MyMain::Frame()
{
    for (auto manager : m_StateManagerMap)
    {
        manager.second->Frame();
    }

    if (I_Input.GetKey(VK_ESCAPE) == KEY_PUSH)
        m_bGameRun = false;

    if (m_pMainCamera)
    {
        m_pMainCamera->Frame();

        Player::GetInstance().SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    }

    m_pQuadTree->Update();
    Player::GetInstance().Frame();

    TVector3 p = Player::GetInstance().m_vPos;
    p.y += 5.0f;
    Player::GetInstance().m_pTrail->AddTrailPos(Player::GetInstance().m_vPos, p);

    m_pEnemy->Frame();

    //modelBox.UpdateBox(Player::GetInstance().m_matWorld);
    return true;
}

bool    MyMain::Render()
{
    m_pQuadTree->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    m_pQuadTree->Render();

    //TMatrix matWorld = TMatrix::Identity;
    //Player::GetInstance().SetMatrix(&matWorld, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    //Player::GetInstance().Frame();
    //Player::GetInstance().Render();
    Player::GetInstance().m_pTrail->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    Player::GetInstance().m_pTrail->Render();

    m_pEnemy->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    m_pEnemy->Render();

    if (m_pDebugBox)
    {
        m_pDebugBox->SetMatrix(&m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
        TColor color = TColor(0, 0, 1, 1);
        if (TCollision::ChkOBBToOBB(Player::GetInstance().m_ColliderBox, testBox))
        {
            color = TColor(1, 0, 0, 1);
            TVector3 n = TVector3(0, 0, 1);
            //TVector3 L = -Player::GetInstance().m_vDirection;
            //Player::GetInstance().m_vPos += L * 15.0f * g_fSecondPerFrame;
            //Player::GetInstance().UpdateMatrix();
            //Player::GetInstance().UpdateBuffer();
            //Player::GetInstance().Render();
        }
        for (T_BOX* box : m_debugBoxList)
        {
            m_pDebugBox->SetBox(*box);
            m_pDebugBox->SetColor(color);
            m_pDebugBox->UpdateBuffer();
            m_pDebugBox->Render();
        }

        T_BOX b;
        b.CreateOBBBox(0.2, 0.2, 0.2, Player::GetInstance().GetCurSocketPos("WeaponLow"));
        m_pDebugBox->SetBox(b);
        m_pDebugBox->SetColor(TColor(1, 1, 1, 1));
        m_pDebugBox->UpdateBuffer();
        //m_pDebugBox->Render();

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
    }

    return true;
}

bool    MyMain::Release()
{
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
    
    if (m_pEnemy)
    {
        m_pEnemy->Release();
        delete m_pEnemy;
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

GAME_RUN(MyMain, PROJECTNAME, 1600, 900)