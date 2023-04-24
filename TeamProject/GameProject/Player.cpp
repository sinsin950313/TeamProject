#include "Player.h"
#include "Input.h"
#include "CollisionMgr.h"

Player::Player() : m_Tornado(this)
{

}

Player::~Player()
{
	Release();
}

bool    Player::Init()
{
	Character::Init();

	for (int i = 0; i < 12; i++)
	{
		D3DXMatrixTranspose(&m_cbInstancingData.matWorld[i], &m_matWorld);
		float alpha = (1 - (i / (float)12)) * 0.5f;
		m_vInstancingColor[i] = { 0.2, 0.1, 0.8, alpha * alpha };
	}

	m_pInstancingBuffer = CreateConstantBuffer(m_pd3dDevice, &m_cbInstancingData, sizeof(VS_INSTANCING_BUFFER));
	I_Shader.VSLoad(L"../../data/shader/VSInstancingModel.hlsl", L"VS", &m_pModelVS[0]);
	I_Shader.VSLoad(L"../../data/shader/SSB/Default3DVertexShader_PCNT_Skinning.hlsl", L"VS", &m_pModelVS[1]);

	I_Shader.PSLoad(L"../../data/shader/PSInstancingModel.hlsl", L"COLOR_PS", &m_pModelPS[0]);
	I_Shader.PSLoad(L"../../data/shader/PSInstancingModel.hlsl", L"PS", &m_pModelPS[1]);

	m_AttackBox.CreateOBBBox(1, 1, 1);

	m_pTrail = new TrailEffect();
	m_pTrail->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/TrailEffect.txt", L"../../data/swoosh.png");
	m_pTrail->Init();

	m_Tornado.SetDevice(m_pd3dDevice, m_pImmediateContext);

	return true;
}

bool    Player::Frame()
{
	static float timer = 0.0f;
	timer += g_fSecondPerFrame;
	if (timer > 0.005f)
	{
		m_matInstancing.push_back(m_matWorld);
		if (m_matInstancing.size() > 12)
		{
			m_matInstancing.erase(m_matInstancing.begin());
		}
		timer = 0.0f;
	}
	static float color = 0.0f;

	if (I_Input.GetKey(VK_UP) == KEY_HOLD)
	{
		color = min(color + g_fSecondPerFrame, 1.0f);
	}
	else if (I_Input.GetKey(VK_DOWN) == KEY_HOLD)
	{
		color = max(color - g_fSecondPerFrame, 0.0f);
	}

	//for (int i = 0; i < 12; i++)
	//{
	//	m_vInstancingColor[i].x = color;
	//	m_vInstancingColor[i].y = color;
	//	m_vInstancingColor[i].z = color;
	//}

	Character::Frame();
	UpdateInstancingBuffer();
	m_pTrail->Frame();

	if (!m_Tornado.IsFinished())
	{
		m_Tornado.Frame();
	}

	return true;
}

bool Player::Render()
{
	Character::Render();

	if (!m_Tornado.IsFinished())
	{
		if (&m_matWorld)
		{
			m_Tornado.m_matWorld = m_matWorld;
		}
		if (&m_matView)
		{
			m_Tornado.m_matView = m_matView;
		}
		if (&m_matProj)
		{
			m_Tornado.m_matProj = m_matProj;
		}
		m_Tornado.UpdateBuffer();
		m_Tornado.Render();
	}

	return false;
}

bool	Player::PostRender()
{
	if (m_IsDash)
	{
		m_pImmediateContext->VSSetConstantBuffers(0, 1, &_toViewSpaceTransformBuffer);
		m_pImmediateContext->VSSetConstantBuffers(1, 1, &_objectToWorldTransformBuffer);
		m_pImmediateContext->VSSetConstantBuffers(8, 1, &m_pInstancingBuffer);

		m_pImmediateContext->VSSetShader(m_pModelVS[1]->m_pVS, NULL, 0);
		m_pImmediateContext->PSSetShader(m_pModelPS[1]->m_pPS, NULL, 0);
		m_pModel->PostRender();

		m_pImmediateContext->VSSetShader(m_pModelVS[0]->m_pVS, NULL, 0);
		m_pImmediateContext->PSSetShader(m_pModelPS[0]->m_pPS, NULL, 0);
		m_pModel->RenderInstancing(12);

		m_pImmediateContext->VSSetShader(m_pModelVS[1]->m_pVS, NULL, 0);
		m_pImmediateContext->PSSetShader(m_pModelPS[1]->m_pPS, NULL, 0);
		m_pModel->PostRender();
	}

	// 인스턴싱 쪼개서 11 + 1 사이에 PS 갈아치우기

	//m_pModel->Render();


	return true;
}

bool	Player::Release()
{
	if (m_pInstancingBuffer)
	{
		m_pInstancingBuffer->Release();
		m_pInstancingBuffer = nullptr;
	}
	if (m_pTrail)
	{
		m_pTrail->Release();
		delete m_pTrail;
		m_pTrail = nullptr;
	}

	m_Tornado.Release();

	Character::Release();

	return true;
}

void    Player::UpdateInstancingBuffer()
{
	for (int i = 0; i < m_matInstancing.size(); i++)
	{
		D3DXMatrixTranspose(&m_cbInstancingData.matWorld[11 - i], &m_matInstancing[i]);
	}
	for (int i = 0; i < 12; i++)
	{
		m_cbInstancingData.vColor[i] = m_vInstancingColor[i];
	}

	m_pImmediateContext->UpdateSubresource(m_pInstancingBuffer, 0, nullptr, &m_cbInstancingData, 0, 0);
}

void Player::Damage(int damage)
{
	if (!m_IsImmortal)
	{
		Character::Damage(damage);
	}
}

void Player::UltimateSkillStacking(float timeStampe)
{
	if (m_stackingTimeStamp != timeStampe)
	{
		m_stackingTimeStamp = timeStampe;
		m_UltimateSkillStack = min(m_kMaxUltimateSkillStackCount, m_UltimateSkillStack + 1);
	}

	if (m_UltimateSkillStack == m_kMaxUltimateSkillStackCount)
	{
		m_UltimateSkillStack = 0;
	}
}

int Player::GetUltimateSkillStack()
{
	return m_UltimateSkillStack;
}

bool Player::IsAbleToCallUltimateSkill()
{
	return m_kMaxUltimateSkillStackCount == m_UltimateSkillStack;
}

void Player::CallUltimateSkill()
{
	m_UltimateSkillStack = 0;
}

void Player::SetMap(MeshMap* pMap)
{
	Character::SetMap(pMap);
	m_Tornado.SetMap(pMap);
}

void Player::ShotTornado(float timestamp)
{
	TVector3 position = { m_matWorld._41, m_matWorld._42, m_matWorld._43 };
	m_Tornado.Initialize_SetPosition(position);

	TVector3 dir = { -m_matWorld._31, -m_matWorld._32, -m_matWorld._33 };
	m_Tornado.Initialize_SetDirection(dir);

	m_Tornado.Initialize_SetTimestamp(timestamp);
	m_Tornado.Init();
}

bool Player::IsUlitmateSkillCallable()
{
	return !m_Tornado.GetAirborneList().empty();
}

std::vector<Character*> Player::GetUltimateSkillTargetList()
{
	return m_Tornado.GetAirborneList();
}

void Player::SetBegin()
{
	_isBegin = true;
}

bool Player::IsBegin()
{
	return _isBegin;
}

void Player::SetVictory()
{
	_isVictory = true;
}

bool Player::IsVictory()
{
	return _isVictory;
}

Player::Tornado::Tornado(Character* owner) : m_Owner(owner)
{
	m_pDebugBox = new DebugBox;
	m_collideBox.CreateOBBBox(2, 6, 2);
}

void Player::Tornado::Initialize_SetPosition(TVector3 position)
{
	m_vPos = position;
}

void Player::Tornado::Initialize_SetDirection(TVector3 direction)
{
	m_Direction = direction;
}

void Player::Tornado::Initialize_SetTimestamp(float timestamp)
{
	m_TimeStamp = timestamp;
}

bool Player::Tornado::IsHit()
{
	return IsFinished() ? false : I_Collision.ChkPlayerAttackToNpcList(&m_collideBox);
}

std::vector<Character*> Player::Tornado::GetHitList()
{
	if (!IsFinished())
	{
		if (I_Collision.ChkPlayerAttackToNpcList(&m_collideBox))
		{
			auto list = I_Collision.GetHitCharacterList(&m_collideBox);
			return list;
		}
	}
	return {};
}

bool Player::Tornado::IsFinished()
{
	return g_fGameTimer > m_TimeStamp + m_Livetime;
}

std::vector<Character*> Player::Tornado::GetAirborneList()
{
	std::vector<Character*> ret;
	for (auto elem : m_DamagedCharacters)
	{
		ret.push_back(elem);
	}
	return ret;
}

bool Player::Tornado::Init()
{
	if (m_pDebugBox != nullptr)
	{
		m_pDebugBox->Release();
	}
	m_pDebugBox->Create(m_pd3dDevice, m_pImmediateContext);
	m_DamagedCharacters.clear();

	m_pModel = new SSB::Model;
	SSB::OBBData data;
	data.Width = 2;
	data.Height = 6;
	data.Depth = 2;
	m_pModel->Initialize_SetBoundingVolume(data);
	m_pModel->Init();

	return true;
}

bool Player::Tornado::Frame()
{
	UpdateMatrix();
	UpdateBuffer();

	XMVECTOR dir = m_Direction;
	XMMATRIX world = XMLoadFloat4x4(&m_matWorld);
	MoveChar(dir, world, m_Speed, true);
	m_collideBox.UpdateBox(m_matWorld);

	auto list = GetHitList();
	for (auto obj : list)
	{
		if (obj != m_Owner)
		{
			if (m_DamagedCharacters.find(obj) == m_DamagedCharacters.end())
			{
				m_DamagedCharacters.insert(obj);
				obj->Damage(m_Damage);
				obj->SetAirborne();
				float currentHp = obj->m_HealthPoint - m_Damage;
				if (currentHp <= 0)
					currentHp = 0;
				obj->m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage(currentHp / obj->m_kHealthPointMax, 1.0f));
				obj->m_pInterDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_Damage, obj->m_pInterDamage, 0.5f, 10.0f));
				//obj->m_pGageHP->m_pWorkList.push_back(new InterfaceSetGage((float)obj->m_HealthPoint / obj->m_HealthPointMax, 1.0f));
				//obj->m_pDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_Damage, obj->m_pDamage, 0.5f, 10.0f));
			}
		}
	}

	std::vector<Character*> erased;
	for (auto obj : m_DamagedCharacters)
	{
		if (!obj->IsAirborne())
		{
			erased.push_back(obj);
		}
	}

	for (auto erase : erased)
	{
		m_DamagedCharacters.erase(erase);
	}

	return true;
}

bool Player::Tornado::Render()
{
	m_pDebugBox->SetMatrix(&m_matView, &m_matProj);
	TColor color = TColor(1, 1, 1, 1);
	{
		m_pDebugBox->SetBox(m_collideBox);
		m_pDebugBox->SetColor(color);
		m_pDebugBox->UpdateBuffer();
		m_pDebugBox->Render();
	}

	return true;
}

bool Player::Tornado::Release()
{
	if (m_pDebugBox)
	{
		m_pDebugBox->Release();
		delete m_pDebugBox;
		m_pDebugBox = nullptr;
	}

	Character::Release();

	return true;
}
