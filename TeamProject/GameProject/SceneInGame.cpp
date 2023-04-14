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

	MapLoad();
	
	UiLoad();

	FSMLoad();

	CharacterLoad();

}


bool    SceneInGame::Init()
{
	I_Input.SwitchShowMouse(false);

	m_debugBoxList.push_back(&Player::GetInstance().m_ColliderBox);
	m_debugBoxList.push_back(&Player::GetInstance().m_AttackBox);

	for (auto enemy : m_Enemies)
	{
		m_debugBoxList.push_back(&enemy->m_ColliderBox);
		m_debugBoxList.push_back(&enemy->m_AttackBox);
	}

	m_debugBoxList.push_back(&m_pBoss->m_ColliderBox);
	m_debugBoxList.push_back(&m_pBoss->m_AttackBox);

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
		enemy->m_pGageHP->SetAttribute({ enemy->m_vPos.x, enemy->m_vPos.y + 2, enemy->m_vPos.z }, {0.005, 0.01, 0.01});
		enemy->m_pGageHP->Frame();

		enemy->m_pDamage->SetAttribute({ enemy->m_vPos.x, enemy->m_vPos.y + 2.5f, enemy->m_vPos.z }, { 0.01, 0.01, 0.01 });
		enemy->m_pDamage->Frame();
	}

	if (m_pBoss)
	{
		m_pBoss->Frame();
	}
	m_pInter_MinimapContents->Frame();
	m_pInter_Ingame->Frame();
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
		enemy->m_pGageHP->SetMatrix(nullptr, &enemy->m_matView, &enemy->m_matProj);
		enemy->m_pGageHP->Render();

		enemy->m_pDamage->SetMatrix(nullptr, &enemy->m_matView, &enemy->m_matProj);
		enemy->m_pDamage->Render();
	}

	if (m_pBoss)
	{
		m_pBoss->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
		m_pBoss->Render();
	}

	//if (m_pDebugBox)
	//{
	//	//for (auto box : I_Collision.GetMapCollisionList())
	//	//{
	//	//    m_pDebugBox->SetMatrix(&m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
	//	//    m_pDebugBox->SetBox(box);
	//	//    m_pDebugBox->SetColor({1, 0, 0, 1});
	//	//    m_pDebugBox->UpdateBuffer();
	//	//    m_pDebugBox->Render();
	//	//}

	//	m_pDebugBox->SetMatrix(&m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
	//	TColor color = TColor(0, 0, 1, 1);
	//	for (T_BOX* box : m_debugBoxList)
	//	{
	//		m_pDebugBox->SetBox(*box);
	//		m_pDebugBox->SetColor(color);
	//		m_pDebugBox->UpdateBuffer();
	//		m_pDebugBox->Render();
	//	}

	//	T_BOX b;
	//	b.CreateOBBBox(0.2, 0.2, 0.2, Player::GetInstance().GetCurSocketPos("WeaponHigh"));
	//	m_pDebugBox->SetBox(b);
	//	m_pDebugBox->SetColor(TColor(1, 1, 1, 1));
	//	m_pDebugBox->UpdateBuffer();
	//	//m_pDebugBox->Render();

	//	TVector3 cen = (Player::GetInstance().GetCurSocketPos("WeaponLow") + b.vCenter) * 0.5;
	//	TVector3 l = (Player::GetInstance().GetCurSocketPos("WeaponLow") - cen);
	//	float ex = D3DXVec3Length(&l);

	//	TVector3 axis[3];
	//	axis[0] = -l;
	//	D3DXVec3Normalize(&axis[0], &axis[0]);

	//	TVector3 B;
	//	D3DXVec3Cross(&B, &axis[0], &TVector3::UnitX);
	//	if (B == TVector3::Zero)
	//	{
	//		D3DXVec3Cross(&B, &axis[0], &TVector3::UnitY);
	//	}
	//	TVector3 C;
	//	D3DXVec3Cross(&C, &axis[0], &B);

	//	b.CreateOBBBox(ex, 0.3, 0.3, cen, axis[0], B, C);
	//	m_pDebugBox->SetBox(b);
	//	m_pDebugBox->SetColor(TColor(1, 1, 1, 1));
	//	m_pDebugBox->UpdateBuffer();
	//	//m_pDebugBox->Render();

	//	//m_pDebugBox->SetBox(TVector3(0, 0, 0), TVector3::Zero, TVector3::One);
	//	//T_BOX box;
	//	//box.CreateOBBBox();
	//	//m_pDebugBox->Render();
	//}


	Player::GetInstance().m_pTrail->SetMatrix(nullptr, &m_pMainCamera->m_matView, &m_pMainCamera->m_matProj);
	Player::GetInstance().m_pTrail->Render();

	RenderMinimap();

	m_pInter_Ingame->Render();

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
    m_pInter_Ingame->Render();

	return true;
}

bool    SceneInGame::Release()
{
	if (m_pInter_MinimapContents)
	{
		m_pInter_MinimapContents->Release();
		delete m_pInter_MinimapContents;
		m_pInter_MinimapContents = nullptr;
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

	Player::GetInstance().m_pGageHP = nullptr;

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

	if (m_pBoss)
	{
		m_pBoss->Release();
		delete m_pBoss;
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

void    SceneInGame::CameraLoad()
{
	m_pMainCamera = new CameraTPS;
	m_pMainCamera->CreateViewMatrix(TVector3(0, 10, -30), TVector3(0, 0, 0.1f), TVector3(0, 1, 0));
	m_pMainCamera->CreateProjMatrix(0.1f, 1500.0f, XM_PI * 0.25f, (float)g_rcClient.right / (float)g_rcClient.bottom);

	m_pMinimapCamera = new Camera();
	m_pMinimapCamera->CreateViewMatrix(TVector3(0, 400, 0), TVector3(0, 0, 0.1f), TVector3(0, 0, 1));
	m_pMinimapCamera->CreateProjMatrix(0.1f, 1500.0f, XM_PI * 0.25f, 300.0f/ 300.0f);
}

void    SceneInGame::CharacterLoad()
{
	{
		SSB::ObjectScriptIO io;
		std::string filename = "Yasuo";
		//std::string str = io.Read(filename);

		Player::GetInstance().SetDevice(m_pd3dDevice, m_pImmediateContext);
		Player::GetInstance().m_pMainCamera = m_pMainCamera;
		((CameraTPS*)m_pMainCamera)->m_vFollowPos = &Player::GetInstance().m_vPos;

		//Idle, Attack1, Attack2, Attack3, Move, Dead
		I_Model.Load(filename, "Idle", &Player::GetInstance().m_pModel);

		Player::GetInstance().Initialize_SetPosition(TVector3(0, 0, 0));
		//Player::GetInstance()._damagedSound = I_Sound.Find(L"GarenDamaged.mp3");
		Player::GetInstance().m_Damage = 100;
		Player::GetInstance().Initialize_RegisterSkill(SSB::kPlayerDash, 5);
		Player::GetInstance().Initialize_RegisterSkill(SSB::kPlayerSkill1, 8);
		Player::GetInstance().Initialize_RegisterSkill(SSB::kPlayerSkill2, 8);
		Player::GetInstance().Init();
		Player::GetInstance().Scale(0.01f);

		m_StateManagerMap.find(SSB::kPlayerStateManager)->second->RegisterCharacter(&Player::GetInstance(), SSB::kPlayerIdle);

		Player::GetInstance().SetMap(m_pQuadTree->m_pMap);
		Player::GetInstance().m_pGageHP = m_pInter_PlayerHP;
		Player::GetInstance().m_pMinimapProfile = m_pInter_Minimap_player;
	}

	{
		SSB::ObjectScriptIO io;

		std::string str = "Alistar";

		for (int i = 0; i < m_pQuadTree->m_EnemySpawnList.size(); ++i)
		{
			SSB::EnemyNPCMob* enemy = new SSB::EnemyNPCMob();
			enemy->SetDevice(m_pd3dDevice, m_pImmediateContext);
			I_Model.Load(str, "Idle", &enemy->m_pModel);

			XMFLOAT3 pos;
			XMStoreFloat3(&pos, m_pQuadTree->m_EnemySpawnList[i].position);
			enemy->Initialize_SetPosition({ pos.x, pos.y, pos.z });
			enemy->m_Damage = 10;
			enemy->m_fSpeed = 10;
			//enemy->_damagedSound = I_Sound.Find(L"AlistarDamaged.mp3");
			enemy->Init();
			enemy->Scale(0.01f);

			m_StateManagerMap.find(SSB::kEnemyNPCMobStateManager)->second->RegisterCharacter(enemy, SSB::kEnemyNPCMobIdle);

			enemy->SetMap(m_pQuadTree->m_pMap);

			enemy->m_pGageHP = new InterfaceBillboard();
			enemy->m_pGageHP->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/enemy_hp.dds");

			InterfaceDamage* pDamage = new InterfaceDamage();
			pDamage->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/damage_font.dds");
			pDamage->SetDamageList(&m_DamageFontList);
			enemy->m_pDamage = pDamage;
			
			InterfaceMinimap* pMinimapEnemy = new InterfaceMinimap();
			pMinimapEnemy->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/skill_q.dds");
			pMinimapEnemy->SetMapDesc(m_pQuadTree->m_pMap->m_dwNumColumns, m_pQuadTree->m_pMap->m_dwNumRows);
			m_pInter_MinimapContents->AddChild(pMinimapEnemy);
			enemy->m_pMinimapProfile = pMinimapEnemy;

			m_Enemies.push_back(enemy);
		}
	}
	{
		SSB::ObjectScriptIO io;

		std::string str = "Herald";

		m_pBoss = new SSB::BossMob();
		m_pBoss->SetDevice(m_pd3dDevice, m_pImmediateContext);
		I_Model.Load(str, "Spawn", &m_pBoss->m_pModel);

		m_pBoss->Initialize_SetPosition({ 0, 0, -50 });
		m_pBoss->m_Damage = 10;
		m_pBoss->m_fSpeed = 10;
		//enemy->_damagedSound = I_Sound.Find(L"AlistarDamaged.mp3");
		m_pBoss->Init();
		m_pBoss->Scale(0.01f);

		m_StateManagerMap.find(SSB::kBossMobStateManager)->second->RegisterCharacter(m_pBoss, SSB::kBossMobSpawn);

		m_pBoss->SetMap(m_pQuadTree->m_pMap);
	}
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

	Interface* pInter_Profile = new Interface();
	pInter_Profile->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/profile.dds");
	pInter_Profile->SetAttribute(TVector3(570, 787, 0));
	m_pInter_Ingame->AddChild(pInter_Profile);

	Interface* pInter_Frame = new Interface();
	pInter_Frame->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/frame2.dds");
	pInter_Frame->SetAttribute(TVector3(0, 0, 0));
	m_pInter_Ingame->AddChild(pInter_Frame);
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
	m_pInter_Skill_Q->SetAttribute(TVector3(726, 788, 0));
	m_pInter_Skill_Q->m_pWorkList.push_back(new InterfaceFadeClockwise(5.0f));
	m_pInter_Ingame->AddChild(m_pInter_Skill_Q);

	m_pInter_Skill_W = new Interface();
	m_pInter_Skill_W->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/skill_w.dds");
	m_pInter_Skill_W->SetAttribute(TVector3(780, 788, 0));
	m_pInter_Ingame->AddChild(m_pInter_Skill_W);

	m_pInter_Skill_E = new Interface();
	m_pInter_Skill_E->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/skill_e.dds");
	m_pInter_Skill_E->SetAttribute(TVector3(837, 788, 0));
	m_pInter_Ingame->AddChild(m_pInter_Skill_E);

	m_pInter_Skill_R = new Interface();
	m_pInter_Skill_R->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/skill_r.dds");
	m_pInter_Skill_R->SetAttribute(TVector3(892, 788, 0));
	m_pInter_Ingame->AddChild(m_pInter_Skill_R);

	m_pInter_PlayerHP = new Interface();
	m_pInter_PlayerHP->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/player_hp.dds");
	m_pInter_PlayerHP->SetAttribute(TVector3(686, 856, 0));
	m_pInter_Ingame->AddChild(m_pInter_PlayerHP);

	Interface* pInter_HP_Enemy = new Interface();
	pInter_HP_Enemy->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/enemy_hp.dds");
	pInter_HP_Enemy->SetAttribute(TVector3(544, 35, 0));
	m_pInter_Ingame->AddChild(pInter_HP_Enemy);

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

	Interface* pInter_MinimapFrame = new Interface();
	pInter_MinimapFrame->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/Ui.txt", L"../../data/UI/minimap.dds");
	pInter_MinimapFrame->SetAttribute(TVector3(952, 778, 0));
	m_pInter_Ingame->AddChild(pInter_MinimapFrame);
}

void    SceneInGame::FSMLoad()
{
	// Register State Manager
	{
		SSB::CharacterStateManager* manager = new SSB::CharacterStateManager;

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
			SSB::CharacterState* state = new SSB::PlayerSkillState1(1.0f);
			state->Initialize_SetStateAnimation("Skill1");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoSkill1.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerSkill1, state);
		}
		{
			SSB::CharacterState* state = new SSB::PlayerSkillState2(1.0f);
			state->Initialize_SetStateAnimation("Skill2");
			state->Initialize_SetEffectSound(I_Sound.Find(L"YasuoSkill2.mp3"));
			manager->Initialize_RegisterState(SSB::kPlayerSkill2, state);
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
	m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/map_normal_1.map", m_pd3dDevice, m_pImmediateContext);
	//m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/map_boss_1.map", m_pd3dDevice, m_pImmediateContext);
	//m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/boss_1_2.map", m_pd3dDevice, m_pImmediateContext);
	//m_pQuadTree = MAPLOAD::OpenMap(L"../../data/map/temp_8_8.map", m_pd3dDevice, m_pImmediateContext);
	m_pQuadTree->m_pCurrentCamera = m_pMainCamera;
}

void SceneInGame::RenderMinimap()
{
	// OMGetRenderTargets를 사용시 Interface의 참조수가 하나씩 증가한다고 함. 현 구조에서 이 코드의 필요 용도를 알 수 없으므로 일단 주석처리하지만 필요하다면 이를 고려하여 다시 구성할 것.
	// https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-omgetrendertargets
	//m_pImmediateContext->OMGetRenderTargets(1, &m_RenderTargetMinimap.m_pOldRTV, &m_RenderTargetMinimap.m_pOldDSV);

	UINT viewportCount = 1;
	m_pImmediateContext->RSGetViewports(&viewportCount, m_RenderTargetMinimap.m_vpOld);
	if(m_RenderTargetMinimap.Begin(m_pImmediateContext))
	{
		m_pQuadTree->SetMatrix(nullptr, &m_pMinimapCamera->m_matView, &m_pMinimapCamera->m_matProj);
		m_pQuadTree->Render();

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
			m_pDebugBox->SetMatrix(&m_pMinimapCamera->m_matView, &m_pMinimapCamera->m_matProj);
			TColor color;
			T_BOX box;
			color = TColor(0, 1, 0, 1);
			box.CreateOBBBox(10.0f, 10.0f, 10.0f, Player::GetInstance().GetPosition());
			m_pDebugBox->SetBox(box);
			m_pDebugBox->SetColor(color);
			m_pDebugBox->UpdateBuffer();
			m_pDebugBox->Render();

			color = TColor(1, 0, 0, 1);
			for (auto enemy : m_Enemies)
			{
				box.CreateOBBBox(10.0f, 10.0f, 10.0f, enemy->GetPosition());
				m_pDebugBox->SetBox(box);
				m_pDebugBox->SetColor(color);
				m_pDebugBox->UpdateBuffer();
				m_pDebugBox->Render();
			}
			color = TColor(1, 1, 0, 1);
			if (m_pBoss)
			{
				box.CreateOBBBox(10.0f, 10.0f, 10.0f, m_pBoss->GetPosition());
				m_pDebugBox->SetBox(box);
				m_pDebugBox->SetColor(color);
				m_pDebugBox->UpdateBuffer();
				m_pDebugBox->Render();
			}
		}
		
		m_RenderTargetMinimap.End(m_pImmediateContext);
	}
	if (m_RenderTargetMinimap.m_pSRV)
	{
		m_pInter_Minimap->m_pTextureSRV = m_RenderTargetMinimap.m_pSRV.Get();
	}
}


