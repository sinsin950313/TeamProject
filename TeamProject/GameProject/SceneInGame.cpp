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
#include "BossStateService.h"
#include "DirectionalLight.h"
#include "LightManager.h"
#include "EffectMgr.h"

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
	
	if (m_bInteractNextStage && I_Collision.IsCollideTrigger(&Player::GetInstance().m_ColliderBox))
	{
		m_Scene = S_INGAME2;
		I_Collision.GetMapCollisionList().clear();
	}
	return m_Scene;
}

SceneInGame::~SceneInGame()
{
}

void SceneInGame::DataLoad()
{
    SSB::DirectionalLight* light = new SSB::DirectionalLight;
    light->Initialize_SetDevice(m_pd3dDevice, m_pImmediateContext);
    light->Init();
    light->SetLightDirection(TVector3(0, -1, 1));
    light->SetLightPosition(TVector3(0, 100, -100));
    SSB::I_Light.GetInstance().SetLight(light);

    I_Model.SetDevice(m_pd3dDevice, m_pImmediateContext);

	I_Sound.LoadDir(kTeamProjectSoundPath);
	I_Sound.LoadAll(kTeamProjectSoundPath);

	CameraLoad();
	I_Effect.SetDevice(m_pd3dDevice, m_pImmediateContext);
	I_Effect.SetCamera(m_pMainCamera);

	MapLoad();
	
	UiLoad();

	FSMLoad();

	CharacterLoad();
}


bool    SceneInGame::Init()
{
	I_Input.SwitchShowMouse(false);

	m_debugBoxList.push_back(&Player::GetInstance().m_ColliderBox);
	//m_debugBoxList.push_back(&Player::GetInstance().m_AttackBox);

	for (auto enemy : m_Enemies)
	{
		m_debugBoxList.push_back(&enemy->m_ColliderBox);
		//m_debugBoxList.push_back(&enemy->m_AttackBox);
	}

	//m_debugBoxList.push_back(&m_pBoss->m_ColliderBox);
	//m_debugBoxList.push_back(&m_pBoss->m_AttackBox);

	//testBox.CreateOBBBox(40, 4, 4);
	//m_debugBoxList.push_back(&testBox);
	//I_Collision.AddStaticObjectBox(&testBox, NULL);

	m_pDebugBox = new DebugBox;
	m_pDebugBox->Create(m_pd3dDevice, m_pImmediateContext);

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
	if (I_Input.GetKey('P') == KEY_PUSH)
	{
		I_Effect.CreateEffect(L"path", Player::GetInstance().GetPosition());
	}

	if (I_Input.GetKey('O') == KEY_PUSH)
	{
		m_bStartCamera = true;
		m_pInter_GameTitle->m_pWorkList.push_back(new InterfaceFadeOut(m_pQuadTree->m_fCamMoveDuration / 2.0f));
		m_pInter_GameTitle->m_pWorkList.push_back(new InterfaceLifeTime(m_pQuadTree->m_fCamMoveDuration / 2.0f));
		m_pInter_PressStart->m_pWorkList.push_back(new InterfaceFadeOut(m_pQuadTree->m_fCamMoveDuration / 2.0f));
		m_pInter_PressStart->m_pWorkList.push_back(new InterfaceLifeTime(m_pQuadTree->m_fCamMoveDuration / 2.0f));

		/*for (int idx = 0; idx < m_pInter_Ingame->m_pChildList.size(); idx++)
			m_pInter_Ingame->m_pChildList[idx]->m_pWorkList.push_back(new InterfaceFadeIn(m_pQuadTree->m_fCamMoveDuration / 2.0f));
		for (int idx = 0; idx < m_pInter_MinimapContents->m_pChildList.size(); idx++)
			m_pInter_MinimapContents->m_pChildList[idx]->m_pWorkList.push_back(new InterfaceFadeIn(m_pQuadTree->m_fCamMoveDuration / 2.0f));*/
		/*m_pInter_Ingame->SetAllWorkList(new InterfaceFadeIn(m_pQuadTree->m_fCamMoveDuration / 2.0f));
		m_pInter_MinimapContents->SetAllWorkList(new InterfaceFadeIn(m_pQuadTree->m_fCamMoveDuration / 2.0f));*/
	}

	if (m_bStartCamera && m_pQuadTree->m_fCamMoveCurrent <= m_pQuadTree->m_fCamMoveDuration)
	{
		XMFLOAT3 movePos;
		XMFLOAT3 moveDir;
		m_pQuadTree->m_fCamMoveCurrent += g_fSecondPerFrame;
		m_pCinemaCamera->MoveCameraBezierSpline(m_pQuadTree->m_fCamMoveCurrent, m_pQuadTree->m_fCamMoveDuration, m_pQuadTree->m_CamMoveList, movePos, moveDir);
		m_pCinemaCamera->m_vPos = TVector3(movePos.x, movePos.y, movePos.z);
		m_pCinemaCamera->m_fCameraYawAngle = moveDir.x;
		m_pCinemaCamera->m_fCameraPitchAngle = moveDir.y;
		m_pCinemaCamera->m_fCameraRollAngle = moveDir.z;
	}
	if (m_bStartCamera && m_pQuadTree->m_fCamMoveCurrent > m_pQuadTree->m_fCamMoveDuration && m_pCinemaCamera)
	{
		Player::GetInstance().SetBegin();
		if (m_Scene == S_INGAME)
		{
			for (int idx = 0; idx < m_pInter_Ingame->m_pChildList.size(); idx++)
				m_pInter_Ingame->m_pChildList[idx]->m_pWorkList.push_back(new InterfaceFadeIn(0.5f));
			for (int idx = 0; idx < m_pInter_MinimapContents->m_pChildList.size(); idx++)
				m_pInter_MinimapContents->m_pChildList[idx]->m_pWorkList.push_back(new InterfaceFadeIn(0.5f));
		}
		m_pInter_Ingame->AddChild(m_pInter_Damage_blood);
		m_pMainCamera = m_pCameraTemp;
		m_pQuadTree->m_pCurrentCamera = m_pMainCamera;
		Player::GetInstance().m_pMainCamera = m_pMainCamera;
		((CameraTPS*)m_pMainCamera)->m_vFollowPos = &Player::GetInstance().m_vPos;
		delete m_pCinemaCamera;
		m_pCinemaCamera = nullptr;
	}

	if (I_Input.GetKey('V') == KEY_PUSH)
	{
		Player::GetInstance().SetVictory();
	}

	if (I_Input.GetKey(VK_F3) == KEY_PUSH)
		I_Input.SwitchShowMouse(!I_Input.GetShowMouse());

	for (auto manager : m_StateManagerMap)
	{
		manager.second->Frame();
	}

	if (m_pMainCamera)
	{
		m_pMainCamera->Frame();
		m_pQuadTree->Update();

		m_iMobDeadCount = 0;
		for (auto enemy : m_Enemies)
		{
			if (enemy->IsDead())
			{
				++m_iMobDeadCount;
			}
		}
		if (!m_Enemies.empty() && m_iMobDeadCount == m_Enemies.size() && !m_bInteractNextStage)
		{
			m_bInteractNextStage = true;
			I_Collision.AddMapTriggerBox(m_pQuadTree->m_Trigger);
		}

		if (m_pBoss)
		{
			if (m_pBoss->IsDead())
			{
				m_Win = true;
			}
		}
		
		if (Player::GetInstance().IsDead())
		{
			m_Defeat = true;
		}
	}

	Player::GetInstance().Frame();
	for (auto enemy : m_Enemies)
	{
		enemy->Frame();
		if(typeid(*enemy->m_pInterGageHP) == typeid(InterfaceBillboard))
			enemy->m_pInterGageHP->SetAttribute({ enemy->m_vPos.x, enemy->m_vPos.y + 2, enemy->m_vPos.z }, {0.005, 0.01, 0.01});
		enemy->m_pInterGageHP->Frame();

		enemy->m_pInterDamage->SetAttribute({ enemy->m_vPos.x, enemy->m_vPos.y + 2.5f, enemy->m_vPos.z }, { 0.01, 0.01, 0.01 });
		enemy->m_pInterDamage->Frame();
	}

	//if (m_pBoss)
	//{
	//	m_pBoss->Frame();
	//}
	I_Effect.Frame();
	m_pInter_MinimapContents->Frame();
	m_pInter_Ingame->Frame();
	//m_pInter_Title->Frame();
	//modelBox.UpdateBox(Player::GetInstance().m_matWorld);
	return true;
}

bool SceneInGame::PreRender()
{
	auto lights = SSB::I_Light.GetLightList();
    for (auto light : lights)
    {
        Camera tmp;
        tmp.CreateViewMatrix(light->m_vPos, light->m_vLookAt, TVector3(0, 1, 0));
        tmp.CreateProjMatrix(0.1f, 1500.0f, XM_PI * 0.25f, (float)g_rcClient.right / (float)g_rcClient.bottom);

        m_pQuadTree->SetMatrix(nullptr, &tmp.m_matView, &tmp.m_matProj);
        m_pQuadTree->PreRender();

        Player::GetInstance().SetMatrix(nullptr, &tmp.m_matView, &tmp.m_matProj);
        Player::GetInstance().PreRender();

        for (auto enemy : m_Enemies)
        {
            enemy->SetMatrix(nullptr, &tmp.m_matView, &tmp.m_matProj);
            enemy->PreRender();
        }
    }

    return true;
}

bool    SceneInGame::Render()
{
    m_pImmediateContext->GSSetShader(NULL, NULL, 0);

    m_pQuadTree->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    m_pQuadTree->Render();

	Player::GetInstance().SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
	Player::GetInstance().Render();

	for (auto enemy : m_Enemies)
	{
		enemy->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
		enemy->Render();
	}

	/*if (m_pBoss)
	{
		m_pBoss->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
		m_pBoss->Render();
	}*/

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

		//m_pDebugBox->SetMatrix(&m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
		//TColor color = TColor(0, 0, 1, 1);
		//for (T_BOX* box : m_debugBoxList)
		//{
		//	m_pDebugBox->SetBox(*box);
		//	m_pDebugBox->SetColor(color);
		//	m_pDebugBox->UpdateBuffer();
		//	m_pDebugBox->Render();
		//}

		//for (T_BOX box : I_Collision.GetInstance().GetMapCollisionList())
		//{
		//	m_pDebugBox->SetBox(box);
		//	m_pDebugBox->SetColor(color);
		//	m_pDebugBox->UpdateBuffer();
		//	m_pDebugBox->Render();
		//}

		//T_BOX b;
		//b.CreateOBBBox(0.2, 0.2, 0.2, Player::GetInstance().GetCurSocketPos("WeaponHigh"));
		//m_pDebugBox->SetBox(b);
		//m_pDebugBox->SetColor(TColor(1, 1, 1, 1));
		//m_pDebugBox->UpdateBuffer();
		////m_pDebugBox->Render();

		//TVector3 cen = (Player::GetInstance().GetCurSocketPos("WeaponLow") + b.vCenter) * 0.5;
		//TVector3 l = (Player::GetInstance().GetCurSocketPos("WeaponLow") - cen);
		//float ex = D3DXVec3Length(&l);

		//TVector3 axis[3];
		//axis[0] = -l;
		//D3DXVec3Normalize(&axis[0], &axis[0]);

		//TVector3 B;
		//D3DXVec3Cross(&B, &axis[0], &TVector3::UnitX);
		//if (B == TVector3::Zero)
		//{
		//	D3DXVec3Cross(&B, &axis[0], &TVector3::UnitY);
		//}
		//TVector3 C;
		//D3DXVec3Cross(&C, &axis[0], &B);

		//b.CreateOBBBox(ex, 0.3, 0.3, cen, axis[0], B, C);
		//m_pDebugBox->SetBox(b);
		//m_pDebugBox->SetColor(TColor(1, 1, 1, 1));
		//m_pDebugBox->UpdateBuffer();
		//m_pDebugBox->Render();

		//m_pDebugBox->SetBox(TVector3(0, 0, 0), TVector3::Zero, TVector3::One);
		//T_BOX box;
		//box.CreateOBBBox();
		//m_pDebugBox->Render();
	}


	//Player::GetInstance().m_pTrail->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
	//Player::GetInstance().m_pTrail->Render();

    // Camera의 위치정보가 필요하므로 지우지 말 것
	auto lights = SSB::I_Light.GetLightList();
	for (auto light : lights)
	{
		light->SetMatrix(&m_pMainCamera->m_matWorld, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
	}

    return true;
}

bool SceneInGame::PostRender()
{
	Player::GetInstance().PostRender();

    Player::GetInstance().m_pTrail->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
    Player::GetInstance().m_pTrail->Render();

	for (auto enemy : m_Enemies)
	{
		if (typeid(*enemy->m_pInterGageHP) == typeid(InterfaceBillboard))
			enemy->m_pInterGageHP->SetMatrix(nullptr, &enemy->m_matView, &enemy->m_matProj);
		enemy->m_pInterGageHP->Render();

		enemy->m_pInterDamage->SetMatrix(nullptr, &enemy->m_matView, &enemy->m_matProj);
		enemy->m_pInterDamage->Render();
	}

	I_Effect.Render();

	RenderMinimap();
    m_pInter_Ingame->Render();
	//m_pInter_Title->Render();
	return true;
}

bool    SceneInGame::Release()
{
	I_Effect.Release();
	if (m_pInter_Title)
	{
		m_pInter_Title->Release();
		delete m_pInter_Title;
		m_pInter_Title = nullptr;
	}

	if (m_pInter_MinimapContents)
	{
		m_pInter_MinimapContents->Release();
		delete m_pInter_MinimapContents;
		m_pInter_MinimapContents = nullptr;
	}

	if (m_pCinemaCamera)
	{
		m_pCinemaCamera->Release();
		delete m_pCinemaCamera;
		m_pCinemaCamera = nullptr;
	}

	if (m_pMinimapCamera)
	{
		m_pMinimapCamera->Release();
		delete m_pMinimapCamera;
		m_pMinimapCamera = nullptr;
	}

	if (m_pInter_Ingame)
	{
		m_pInter_Ingame->Release();
		delete m_pInter_Ingame;
		m_pInter_Ingame = nullptr;
	}

	Player::GetInstance().m_pInterGageHP = nullptr;

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
		m_pMainCamera = nullptr;
	}

	for (auto enemy : m_Enemies)
	{
		enemy->Release();
		delete enemy;
	}

	/*if (m_pBoss)
	{
		m_pBoss->Release();
		delete m_pBoss;
	}*/

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

void    SceneInGame::CameraLoad()
{
	m_pCameraTemp = new CameraTPS;
	m_pCameraTemp->CreateViewMatrix(TVector3(0, 10, -30), TVector3(0, 0, 0.1f), TVector3(0, 1, 0));
	m_pCameraTemp->CreateProjMatrix(0.1f, 1500.0f, XM_PI * 0.25f, (float)g_rcClient.right / (float)g_rcClient.bottom);

	m_pMinimapCamera = new Camera();
	m_pMinimapCamera->CreateViewMatrix(TVector3(0, 400, 0), TVector3(0, 0, 0.1f), TVector3(0, 0, 1));
	m_pMinimapCamera->CreateProjMatrix(0.1f, 1500.0f, XM_PI * 0.25f, 300.0f/ 300.0f);

	m_pCinemaCamera = new CameraCinema();
	m_pCinemaCamera->CreateViewMatrix(TVector3(0, 400, 0), TVector3(0, 0, 0.1f), TVector3(0, 0, 1));
	m_pCinemaCamera->CreateProjMatrix(0.1f, 1500.0f, XM_PI * 0.25f, (float)g_rcClient.right / (float)g_rcClient.bottom);
}

void    SceneInGame::CharacterLoad()
{
	std::string PlayerStr = "Player";
	SSB::ObjectScriptIO io;
	std::string filename = "Yasuo";
	if (!Player::GetInstance().m_pModel)
	{
		Player::GetInstance().SetDevice(m_pd3dDevice, m_pImmediateContext);
		I_Model.Load(filename, "HoudgiPlaying", &Player::GetInstance().m_pModel);

		//Idle, Attack1, Attack2, Attack3, Move, Dead
		Player::GetInstance().Initialize_RegisterSkill(SSB::kPlayerDash, 2);
		Player::GetInstance().Initialize_RegisterSkill(SSB::kPlayerPierce, 2);
		Player::GetInstance().Initialize_RegisterSkill(SSB::kPlayerRotate, 8);
		Player::GetInstance().Initialize_RegisterSkill(SSB::kPlayerUltimate, 30);
		Player::GetInstance().Initialize_RegisterSkill(SSB::kPlayerDrink, 30);
		Player::GetInstance().Init();
	}

	{
		//move to mapload
		/*Player::GetInstance().m_pMainCamera = m_pMainCamera;
		((CameraTPS*)m_pMainCamera)->m_vFollowPos = &Player::GetInstance().m_vPos;*/
    
		//Idle, Attack1, Attack2, Attack3, Move, Dead
		XMFLOAT3 playerSpawnPos;
		XMStoreFloat3(&playerSpawnPos, m_pQuadTree->m_PlayerSpawnPoint.second.position);
		Player::GetInstance().Initialize_SetPosition(TVector3(playerSpawnPos));

		//Player::GetInstance()._damagedSound = I_Sound.Find(L"GarenDamaged.mp3");
		Player::GetInstance().m_Damage = 100;
		Player::GetInstance().Scale(0.01f);

		m_StateManagerMap.find(SSB::kPlayerStateManager)->second->RegisterCharacter(&Player::GetInstance(), SSB::kPlayerHoudgiLoop);

		Player::GetInstance().SetMap(m_pQuadTree->m_pMap);
		Player::GetInstance().m_pInterGageHP = m_pInter_PlayerHP;
		Player::GetInstance().m_pInterMinimapProfile = m_pInter_Minimap_player;
		Player::GetInstance().m_pInterSkillQ = m_pInter_Skill_Q;
		Player::GetInstance().m_pInterSkillDash = m_pInter_Skill_W;
		Player::GetInstance().m_pInterSkillE = m_pInter_Skill_E;
		Player::GetInstance().m_pInterSkillR = m_pInter_Skill_R;
		Player::GetInstance().m_pInterDamageBlood = m_pInter_Damage_blood;
	}


	/*if(m_Scene == S_INGAME)*/
	{
		SSB::ObjectScriptIO io;
		std::string mobStr = "Alistar";
		std::string bossStr = "Herald";

		for (int i = 0; i < m_pQuadTree->m_EnemySpawnList.size(); ++i)
		{
			Character* enemy = nullptr;
			if (m_pQuadTree->m_EnemySpawnList[i].first == mobStr)
			{
				enemy = new SSB::EnemyNPCMob();
				enemy->SetDevice(m_pd3dDevice, m_pImmediateContext);
				I_Model.Load(m_pQuadTree->m_EnemySpawnList[i].first, "Idle", &enemy->m_pModel);
				m_StateManagerMap.find(SSB::kEnemyNPCMobStateManager)->second->RegisterCharacter(enemy, SSB::kEnemyNPCMobIdle);	
			}
			else if (m_pQuadTree->m_EnemySpawnList[i].first == bossStr)
			{
				enemy = new SSB::BossMob();
				enemy->SetDevice(m_pd3dDevice, m_pImmediateContext);
				m_pBoss = dynamic_cast<SSB::BossMob*>(enemy);
				I_Model.Load(m_pQuadTree->m_EnemySpawnList[i].first, "Spawn", &m_pBoss->m_pModel);
				m_StateManagerMap.find(SSB::kBossMobStateManager)->second->RegisterCharacter(m_pBoss, SSB::kBossMobSpawn);
			}
			//I_Model.Load(m_pQuadTree->m_EnemySpawnList[i].first, "Idle", &enemy->m_pModel);
			if (!enemy)
				continue;
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, m_pQuadTree->m_EnemySpawnList[i].second.position);
			enemy->Initialize_SetPosition({ pos.x, pos.y, pos.z });
			enemy->m_Damage = 10;
			enemy->m_fSpeed = 10;
			//enemy->_damagedSound = I_Sound.Find(L"AlistarDamaged.mp3");
			enemy->Init();
			enemy->Scale(0.01f);

			enemy->SetMap(m_pQuadTree->m_pMap);
			if (m_pQuadTree->m_EnemySpawnList[i].first == mobStr)
			{
				enemy->m_pInterGageHP = new InterfaceBillboard();
				enemy->m_pInterGageHP->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/enemy_hp.dds");
			}
			else
			{
				m_pInter_BossHP = new Interface();
				m_pInter_BossHP->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/enemy_hp.dds");
				m_pInter_BossHP->SetAttribute(TVector3(544, 35, 0));
				//m_pInter_Ingame->AddChild(m_pInter_BossHP);
				enemy->m_pInterGageHP = m_pInter_BossHP;
			}

			InterfaceDamage* pDamage = new InterfaceDamage();
			pDamage->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/damage_font.dds");
			pDamage->SetDamageList(&m_DamageFontList);
			enemy->m_pInterDamage = pDamage;
			
			InterfaceMinimap* pMinimapEnemy = new InterfaceMinimap();
			pMinimapEnemy->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/skill_q.dds");
			pMinimapEnemy->SetMapDesc(m_pQuadTree->m_pMap->m_dwNumColumns, m_pQuadTree->m_pMap->m_dwNumRows);
			m_pInter_MinimapContents->AddChild(pMinimapEnemy);
			enemy->m_pInterMinimapProfile = pMinimapEnemy;

			m_Enemies.push_back(enemy);
		}
	}
	//if(m_Scene == S_INGAME2)
	//{
	//	SSB::ObjectScriptIO io;

	//	std::string str = "Herald";

	//	m_pBoss = new SSB::BossMob();
	//	m_pBoss->SetDevice(m_pd3dDevice, m_pImmediateContext);
	//	I_Model.Load(str, "Spawn", &m_pBoss->m_pModel);

	//	m_pBoss->Initialize_SetPosition({ 0, 0, -50 });
	//	m_pBoss->m_Damage = 10;
	//	m_pBoss->m_fSpeed = 10;
	//	//enemy->_damagedSound = I_Sound.Find(L"AlistarDamaged.mp3");
	//	m_pBoss->Init();
	//	m_pBoss->Scale(0.01f);

	//	m_StateManagerMap.find(SSB::kBossMobStateManager)->second->RegisterCharacter(m_pBoss, SSB::kBossMobSpawn);

	//	m_pBoss->SetMap(m_pQuadTree->m_pMap);
	//}
	I_Shader.PSLoad(L"../../data/shader/MAP/PSMinimap_Object.hlsl", L"psmain", &m_pMinimapPS_Object);
}

void    SceneInGame::UiLoad()
{
	m_DamageFontList.push_back(DamageFont(0, 3, 50, 30, 40));
	m_DamageFontList.push_back(DamageFont(1, 41, 49, 22, 39));
	m_DamageFontList.push_back(DamageFont(2, 72, 49, 29, 39));
	m_DamageFontList.push_back(DamageFont(3, 107, 49, 29, 40));
	m_DamageFontList.push_back(DamageFont(4, 140, 49, 31, 42));
	m_DamageFontList.push_back(DamageFont(5, 176, 49, 28, 41));
	m_DamageFontList.push_back(DamageFont(6, 211, 50, 30, 40));
	m_DamageFontList.push_back(DamageFont(7, 247, 51, 27, 41));
	m_DamageFontList.push_back(DamageFont(8, 280, 50, 31, 40));
	m_DamageFontList.push_back(DamageFont(9, 315, 50, 30, 40));

	m_pInter_Ingame = new Interface();

	m_pInter_Profile = new Interface();
	m_pInter_Profile->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/profile.dds");
	m_pInter_Profile->SetAttribute(TVector3(570, 787, 0));
	m_pInter_Ingame->AddChild(m_pInter_Profile);

	m_pInter_Frame = new Interface();
	m_pInter_Frame->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/frame2.dds");
	m_pInter_Frame->SetAttribute(TVector3(0, 0, 0));
	m_pInter_Ingame->AddChild(m_pInter_Frame);
	//m_pInter->m_pWorkList.push_back(new InterfaceFade());
	//m_pInter->m_pWorkList.push_back(new InterfaceLoopFade(1.0f));
	//m_pInter->m_pWorkList.push_back(new InterfaceLifeTime(10.0f));
	//m_pInter->m_pWorkList.push_back(new InterfaceClick(m_pInter->m_vScale.x));
	
	m_pInter_Passive = new Interface();
	m_pInter_Passive->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/passive.dds");
	m_pInter_Passive->SetAttribute(TVector3(687, 788, 0));
	m_pInter_Ingame->AddChild(m_pInter_Passive);

	m_pInter_Skill_Q = new Interface();
	m_pInter_Skill_Q->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/skill_q.dds", L"VS", L"ANGLE_PS");
	m_pInter_Skill_Q->m_cbData.fTimer = 360.0f;
	m_pInter_Skill_Q->SetAttribute(TVector3(726, 788, 0));
	/*m_pInter_Skill_Q->m_pWorkList.push_back(new InterfaceFadeClockwise(5.0f));*/
	m_pInter_Ingame->AddChild(m_pInter_Skill_Q);

	m_pInter_Skill_W = new Interface();
	m_pInter_Skill_W->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/skill_w.dds", L"VS", L"ANGLE_PS");
	m_pInter_Skill_W->m_cbData.fTimer = 360.0f;
	m_pInter_Skill_W->SetAttribute(TVector3(780, 788, 0));
	m_pInter_Ingame->AddChild(m_pInter_Skill_W);

	m_pInter_Skill_E = new Interface();
	m_pInter_Skill_E->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/skill_e.dds", L"VS", L"ANGLE_PS");
	m_pInter_Skill_E->m_cbData.fTimer = 360.0f;
	m_pInter_Skill_E->SetAttribute(TVector3(837, 788, 0));
	m_pInter_Ingame->AddChild(m_pInter_Skill_E);

	m_pInter_Skill_R = new Interface();
	m_pInter_Skill_R->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/skill_r.dds", L"VS", L"ANGLE_PS");
	m_pInter_Skill_R->m_cbData.fTimer = 360.0f;
	m_pInter_Skill_R->SetAttribute(TVector3(892, 788, 0));
	m_pInter_Ingame->AddChild(m_pInter_Skill_R);

	m_pInter_PlayerHP = new Interface();
	m_pInter_PlayerHP->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/player_hp.dds");
	m_pInter_PlayerHP->SetAttribute(TVector3(686, 856, 0));
	m_pInter_Ingame->AddChild(m_pInter_PlayerHP);

	/*m_pInter_BossHP = new Interface();
	m_pInter_BossHP->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/enemy_hp.dds");
	m_pInter_BossHP->SetAttribute(TVector3(544, 35, 0));
	m_pInter_Ingame->AddChild(m_pInter_BossHP);*/

	m_RenderTargetMinimap.Create(m_pd3dDevice, m_pImmediateContext, 300, 300);

	m_pInter_Minimap = new Interface();
	m_pInter_Minimap->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/minimap.dds");
	m_pInter_Minimap->SetAttribute(TVector3(952, 778, 0));
	m_pInter_Ingame->AddChild(m_pInter_Minimap);

	m_pInter_MinimapContents = new InterfaceMinimap();
	m_pInter_Minimap_player = new InterfaceMinimap();
	m_pInter_Minimap_player->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/profile.dds");
	m_pInter_Minimap_player->SetMapDesc(m_pQuadTree->m_pMap->m_dwNumColumns, m_pQuadTree->m_pMap->m_dwNumRows);
	m_pInter_MinimapContents->AddChild(m_pInter_Minimap_player);

	m_pInter_MinimapFrame = new Interface();
	m_pInter_MinimapFrame->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/minimap.dds");
	m_pInter_MinimapFrame->SetAttribute(TVector3(952, 778, 0));
	m_pInter_Ingame->AddChild(m_pInter_MinimapFrame);

	m_pInter_Damage_blood = new Interface();
	m_pInter_Damage_blood->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/damage_blood.dds");
	m_pInter_Damage_blood->SetAttribute(TVector3(0, 0, 0));

	for (int i = 0; i < m_pInter_Damage_blood->m_VertexList.size(); i++)
		m_pInter_Damage_blood->m_VertexList[i].c.w = 0.0f;

	if (m_Scene == S_INGAME2)
		return;

	m_pInter_Ingame->SetAllAlpha(0.0f);
	m_pInter_MinimapContents->SetAllAlpha(0.0f);

	//m_pInter_Title = new Interface();
	m_pInter_GameTitle = new Interface();
	m_pInter_GameTitle->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/game_logo.dds");
	m_pInter_GameTitle->SetAttribute(TVector3(0, 0, 0));
	m_pInter_Ingame->AddChild(m_pInter_GameTitle);

	m_pInter_PressStart = new Interface();
	m_pInter_PressStart->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/press_start.dds");
	m_pInter_PressStart->SetAttribute(TVector3(267, 600, 0));
	m_pInter_PressStart->m_pWorkList.push_back(new InterfaceLoopFade(1.0f));
	m_pInter_Ingame->AddChild(m_pInter_PressStart);
}

void    SceneInGame::FSMLoad()
{
	const float kUltimateSkillActiveTime = 1.3f;
	// Register State Manager
	{
		SSB::CharacterStateManager* manager = new SSB::CharacterStateManager;

		{
			SSB::CharacterState* state = new SSB::PlayerHoudgiStartState(0.0f);
			state->Initialize_SetStateAnimation("HoudgiPlaying");
			manager->Initialize_RegisterState(SSB::kPlayerHoudgiLoop, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerHoudgiEndState(3.0f);
			state->Initialize_SetStateAnimation("HoudgiEnd");
			manager->Initialize_RegisterState(SSB::kPlayerHoudgiEnd, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerIdleState;
			state->Initialize_SetStateAnimation("Idle");
			manager->Initialize_RegisterState(SSB::kPlayerIdle, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerMoveState;
			state->Initialize_SetStateAnimation("Move");
			state->Initialize_SetEffectSound(I_Sound.Find(L"GarenWalk.mp3"), true);
			manager->Initialize_RegisterState(SSB::kPlayerMove, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerAttackState1(1.0f);
			state->Initialize_SetStateAnimation("Attack1");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoAttack1.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerAttack1, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerAttackState2(1.0f);
			state->Initialize_SetStateAnimation("Attack2");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoAttack2.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerAttack2, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerAttackState3(1.0f);
			state->Initialize_SetStateAnimation("Attack3");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoAttack3.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerAttack3, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerAttackState4(1.0f);
			state->Initialize_SetStateAnimation("Attack4");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoAttack4.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerAttack4, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerSkillPierceState();
			state->Initialize_SetStateAnimation("");
			//state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoSkill1.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerPierce, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerSkillPierceState1(1.0f);
			state->Initialize_SetStateAnimation("Skill2");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoSkill2.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerPierce1, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerSkillPierceState2(1.0f);
			state->Initialize_SetStateAnimation("Skill3");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoSkill2.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerPierce2, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerSkillPierceState3(1.0f);
			state->Initialize_SetStateAnimation("Skill4");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoSkill2.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerPierce3, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerSkillRotate(1.0f);
			state->Initialize_SetStateAnimation("Skill1");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoSkill1.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerRotate, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerUltimateSkillState(kUltimateSkillActiveTime);
			state->Initialize_SetStateAnimation("Skill5");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoSkill3.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerUltimate, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerDrinkSkillState(7.0f);
			state->Initialize_SetStateAnimation("Drink");
			//state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoDrink.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerDrink, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerDashState(0.5f);
			state->Initialize_SetStateAnimation("Dash");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoDash.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerDash, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerDeadState;
			state->Initialize_SetStateAnimation("Dead");
			state->Initialize_SetEffectSound(I_Sound.Find(L"GarenDead.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerDead, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerVictoryStartState(6.7f);
			state->Initialize_SetStateAnimation("RecallStart");
			//state->Initialize_SetEffectSound(I_Sound.Find(L"GarenDead.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerVictoryStart, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerVictoryLoopState;
			state->Initialize_SetStateAnimation("RecallLoop");
			//state->Initialize_SetEffectSound(I_Sound.Find(L"GarenDead.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerVictoryLoop, state);
		}

		manager->Init();
		m_StateManagerMap.insert(std::make_pair(SSB::kPlayerStateManager, manager));
	}

	{
		SSB::CharacterStateManager* manager = new SSB::CharacterStateManager;
		{
			SSB::CharacterState* state = new SSB::EnemyNPCMobIdleState;
			state->Initialize_SetStateAnimation("Idle");
			manager->Initialize_RegisterState(SSB::kEnemyNPCMobIdle, state);
		}
		{
			SSB::CharacterState* state = new SSB::EnemyNPCMobAirBorneState;
			state->Initialize_SetStateAnimation("Idle");
			manager->Initialize_RegisterState(SSB::kEnemyNPCMobAirborne, state);
		}
		{
			SSB::CharacterState* state = new SSB::EnemyNPCMobPoundState(kUltimateSkillActiveTime);
			state->Initialize_SetStateAnimation("Idle");
			manager->Initialize_RegisterState(SSB::kEnemyNPCMobPound, state);
		}
		{
			SSB::CharacterState* state = new SSB::EnemyNPCMobMoveState;
			state->Initialize_SetStateAnimation("Move");
			state->Initialize_SetEffectSound(I_Sound.Find(L"AlistarWalk.mp3"), true);
			manager->Initialize_RegisterState(SSB::kEnemyNPCMobMove, state);
		}
		{
			SSB::CharacterState* state = new SSB::EnemyNPCMobAttack1State(1.5f);
			state->Initialize_SetStateAnimation("Attack1");
			state->Initialize_SetEffectSound(I_Sound.Find(L"AlistarAttack1.mp3"));
			manager->Initialize_RegisterState(SSB::kEnemyNPCMobAttack1, state);
		}
		{
			SSB::CharacterState* state = new SSB::EnemyNPCMobAttack2State(1.5f);
			state->Initialize_SetStateAnimation("Attack2");
			state->Initialize_SetEffectSound(I_Sound.Find(L"AlistarAttack2.mp3"));
			manager->Initialize_RegisterState(SSB::kEnemyNPCMobAttack2, state);
		}
		{
			SSB::CharacterState* state = new SSB::EnemyNPCMobDeadState;
			state->Initialize_SetStateAnimation("Dead");
			state->Initialize_SetEffectSound(I_Sound.Find(L"AlistarDead.mp3"));
			manager->Initialize_RegisterState(SSB::kEnemyNPCMobDead, state);
		}

		manager->Init();
		m_StateManagerMap.insert(std::make_pair(SSB::kEnemyNPCMobStateManager, manager));
	}

	{
		SSB::CharacterStateManager* manager = new SSB::CharacterStateManager;
		{
			SSB::CharacterState* state = new SSB::BossMobIdleState;
			state->Initialize_SetStateAnimation("Idle");
			manager->Initialize_RegisterState(SSB::kBossMobIdle, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobSpawnState(2);
			state->Initialize_SetStateAnimation("Spawn");
			state->Initialize_SetEffectSound(I_Sound.Find(L"BossSpawn.mp3"));
			manager->Initialize_RegisterState(SSB::kBossMobSpawn, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobAngryState(2);
			state->Initialize_SetStateAnimation("Angry");
			state->Initialize_SetEffectSound(I_Sound.Find(L"BossAngry.mp3"));
			manager->Initialize_RegisterState(SSB::kBossMobAngry, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobAirBorneState;
			state->Initialize_SetStateAnimation("Idle");
			manager->Initialize_RegisterState(SSB::kBossMobAirborne, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobPoundState(kUltimateSkillActiveTime);
			state->Initialize_SetStateAnimation("Idle");
			manager->Initialize_RegisterState(SSB::kBossMobPound, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobMoveState;
			state->Initialize_SetStateAnimation("Move");
			state->Initialize_SetEffectSound(I_Sound.Find(L"BossMove.mp3"), true);
			manager->Initialize_RegisterState(SSB::kBossMobMove, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobAttack1State(1.5f);
			state->Initialize_SetStateAnimation("Attack1");
			state->Initialize_SetEffectSound(I_Sound.Find(L"BossAttack1.mp3"));
			manager->Initialize_RegisterState(SSB::kBossMobAttack1, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobAttack2State(1.5f);
			state->Initialize_SetStateAnimation("Attack2");
			state->Initialize_SetEffectSound(I_Sound.Find(L"BossAttack1.mp3"));
			manager->Initialize_RegisterState(SSB::kBossMobAttack2, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobDashStartState(2.0f);
			state->Initialize_SetStateAnimation("DashStart");
			state->Initialize_SetEffectSound(I_Sound.Find(L"BossDashStart.mp3"));
			manager->Initialize_RegisterState(SSB::kBossMobDashStart, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobDashState(1.5f);
			state->Initialize_SetStateAnimation("DashMove");
			state->Initialize_SetEffectSound(I_Sound.Find(L"BossDash.mp3"), true);
			manager->Initialize_RegisterState(SSB::kBossMobDash, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobDashEndState(2);
			state->Initialize_SetStateAnimation("DashEnd");
			state->Initialize_SetEffectSound(I_Sound.Find(L"BossDashEnd.mp3"));
			manager->Initialize_RegisterState(SSB::kBossMobDashEnd, state);
		}
		{
			SSB::CharacterState* state = new SSB::BossMobSkill1State(2);
			state->Initialize_SetStateAnimation("Skill1");
			state->Initialize_SetEffectSound(I_Sound.Find(L"BossSkill1.mp3"));
			manager->Initialize_RegisterState(SSB::kBossMobSkill1, state);
		}
		//{
		//    SSB::CharacterState* state = new SSB::BossMobStunState;
		//    state->Initialize_SetCoolTime(3);
		//    state->Initialize_SetStateAnimation("Stun");
		//    //state->Initialize_SetEffectSound(I_Sound.Find(L"BossStun.mp3"));
		//    manager->Initialize_RegisterState(SSB::kBossMobStun, state);
		//}
		{
			SSB::CharacterState* state = new SSB::BossMobDeadState;
			state->Initialize_SetStateAnimation("Dead");
			state->Initialize_SetEffectSound(I_Sound.Find(L"BossDead.mp3"));
			manager->Initialize_RegisterState(SSB::kBossMobDead, state);
		}

		manager->Init();
		m_StateManagerMap.insert(std::make_pair(SSB::kBossMobStateManager, manager));

		// FMod가 1초 미만의 Sound를 Loop 시 Loop가 안되는 버그가 있음
		// Channel의 문제로 짐작 중
		{
			auto sound = I_Sound.Find(L"BossMove.mp3");
			sound->VolumeDown(100);
			sound->Play(true);
			sound->Stop();
		}
		{
			auto sound = I_Sound.Find(L"BossDash.mp3");
			sound->VolumeDown(100);
			sound->Play(true);
			sound->Stop();
		}
	}
}

void    SceneInGame::MapLoad()
{
	m_pQuadTree = m_Scene == S_INGAME ? MAPLOAD::OpenMap(L"../../data/map/map_normal_1_3_1.map", m_pd3dDevice, m_pImmediateContext) : MAPLOAD::OpenMap(L"../../data/map/map_boss_1_2.map", m_pd3dDevice, m_pImmediateContext);
	//m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/map_boss_1.map", m_pd3dDevice, m_pImmediateContext);
	//m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/boss_1_2.map", m_pd3dDevice, m_pImmediateContext);
	//m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/temp_8_8.map", m_pd3dDevice, m_pImmediateContext);
	
	m_pMainCamera = m_pCinemaCamera;
	m_pQuadTree->m_pCurrentCamera = m_pMainCamera;
	m_pMainCamera->m_vPos = m_pQuadTree->m_CamMoveList[0].camPos;
	m_pMainCamera->m_fCameraYawAngle = m_pQuadTree->m_CamMoveList[0].fYaw;
	m_pMainCamera->m_fCameraPitchAngle = m_pQuadTree->m_CamMoveList[0].fPitch;
	m_pMainCamera->m_fCameraRollAngle = m_pQuadTree->m_CamMoveList[0].fRoll;

	//마지막 카메라이동의 followpos를 받아오기위함
	((CameraTPS*)m_pCameraTemp)->m_vFollowPos = &Player::GetInstance().m_vPos;
	XMFLOAT3 playerSpawnPos;
	XMStoreFloat3(&playerSpawnPos, m_pQuadTree->m_PlayerSpawnPoint.second.position);
	Player::GetInstance().Initialize_SetPosition(TVector3(playerSpawnPos));
	m_pCameraTemp->Frame();

	m_pQuadTree->m_CamMoveList[m_pQuadTree->m_CamMoveList.size() - 1].camPos.x = m_pCameraTemp->m_vPos.x;
	m_pQuadTree->m_CamMoveList[m_pQuadTree->m_CamMoveList.size() - 1].camPos.y = m_pCameraTemp->m_vPos.y;
	m_pQuadTree->m_CamMoveList[m_pQuadTree->m_CamMoveList.size() - 1].camPos.z = m_pCameraTemp->m_vPos.z;
	m_pQuadTree->m_CamMoveList[m_pQuadTree->m_CamMoveList.size() - 1].fYaw = XMConvertToDegrees(m_pCameraTemp->m_fCameraYawAngle - XM_PI);
	m_pQuadTree->m_CamMoveList[m_pQuadTree->m_CamMoveList.size() - 1].fPitch = XMConvertToDegrees(m_pCameraTemp->m_fCameraPitchAngle);
	m_pQuadTree->m_CamMoveList[m_pQuadTree->m_CamMoveList.size() - 1].fRoll = XMConvertToDegrees(m_pCameraTemp->m_fCameraRollAngle);

	I_Shader.PSLoad(L"../../data/shader/MAP/PSMinimap_Map.hlsl", L"psmain", &m_pMinimapPS_Quadtree);
	I_Shader.PSLoad(L"../../data/shader/MAP/PSMinimap_Skydome.hlsl", L"psmain", &m_pMinimapPS_Skydome);
	I_Shader.PSLoad(L"../../data/shader/MAP/PSMinimap_DebugBox.hlsl", L"PS", &m_pMinimapPS_DebugBox);
}

void SceneInGame::RenderMinimap()
{
	Shader* pPSOrigin_Map = nullptr;
	std::map<Object*, Shader*> pPSOrigin_Objects;
	ID3D11PixelShader* pPSOrigin_SkyDome = nullptr;
	ID3D11PixelShader* pPSOrigin_DebugBox = nullptr;
	m_RenderTargetMinimap.m_pOldRTVS = m_pCurrentRenderTargetViews;
	m_RenderTargetMinimap.m_iOldRTVCount = m_iCurrentRTVCount;
	m_RenderTargetMinimap.m_pOldDSV = m_pCurrentDepthStencilView;
	m_RenderTargetMinimap.m_vpOld[0] = *m_pCurrentViewport;
	if(m_RenderTargetMinimap.Begin(m_pImmediateContext))
	{
		Shader* pPSOrigin_Map = m_pQuadTree->m_pPixelShader;
		m_pQuadTree->m_pPixelShader = m_pMinimapPS_Quadtree;
		for (auto iter : m_pQuadTree->m_pAllObjectList)
		{
			pPSOrigin_Objects.insert(std::make_pair(iter, iter->m_pModel->_ps));
			iter->m_pModel->_ps = m_pMinimapPS_Object;
		}
		pPSOrigin_SkyDome = m_pQuadTree->m_pSphereObject->m_pShader->m_pPS;
		m_pQuadTree->m_pSphereObject->m_pShader->m_pPS = m_pMinimapPS_Skydome->m_pPS;

		m_pQuadTree->SetMatrix(nullptr, &m_pMinimapCamera->m_matView, &m_pMinimapCamera->m_matProj);
		m_pQuadTree->Render();

		m_pQuadTree->m_pPixelShader = pPSOrigin_Map;
		for (auto iter : m_pQuadTree->m_pAllObjectList)
		{
			iter->m_pModel->_ps = pPSOrigin_Objects.find(iter)->second;
		}
		m_pQuadTree->m_pSphereObject->m_pShader->m_pPS = pPSOrigin_SkyDome;
		/*Player::GetInstance().m_pMinimapProfile->SetAttribute(Player::GetInstance().GetPosition(), { 0.2f, 0.2f, 0.2f });
		for (auto enemy : m_Enemies)
		{
			enemy->m_pMinimapProfile->SetAttribute(enemy->GetPosition(), { 0.2f, 0.2f, 0.2f });
		}

		if (m_pBoss)
		{
			m_pBoss->m_pMinimapProfile->SetAttribute(m_pBoss->GetPosition());
		}
		m_pInter_MinimapContents->Render();*/

		if (m_pDebugBox)
		{
			pPSOrigin_DebugBox = m_pDebugBox->m_pShader->m_pPS;
			m_pDebugBox->m_pShader->m_pPS = m_pMinimapPS_DebugBox->m_pPS;
			m_pDebugBox->SetMatrix(&m_pMinimapCamera->m_matView, &m_pMinimapCamera->m_matProj);
			TColor color;
			T_BOX box;
			color = TColor(0, 1, 0, 1);
			box.CreateOBBBox(5.0f, 5.0f, 5.0f, Player::GetInstance().GetPosition());
			m_pDebugBox->SetBox(box);
			m_pDebugBox->SetColor(color);
			m_pDebugBox->UpdateBuffer();
			m_pDebugBox->Render();

			color = TColor(1, 0, 0, 1);
			for (auto enemy : m_Enemies)
			{
				if (enemy->IsDead())
					continue;
				box.CreateOBBBox(5.0f, 5.0f, 5.0f, enemy->GetPosition());
				m_pDebugBox->SetBox(box);
				m_pDebugBox->SetColor(color);
				m_pDebugBox->UpdateBuffer();
				m_pDebugBox->Render();
			}
			color = TColor(1, 1, 0, 1);
			if (m_pBoss)
			{
				box.CreateOBBBox(5.0f, 5.0f, 5.0f, m_pBoss->GetPosition());
				m_pDebugBox->SetBox(box);
				m_pDebugBox->SetColor(color);
				m_pDebugBox->UpdateBuffer();
				m_pDebugBox->Render();
			}
			m_pDebugBox->m_pShader->m_pPS = pPSOrigin_DebugBox;
		}
		m_RenderTargetMinimap.End(m_pImmediateContext);
	}
	if (m_RenderTargetMinimap.m_pSRV)
	{
		m_pInter_Minimap->m_pTextureSRV = m_RenderTargetMinimap.m_pSRV.Get();
	}
}


