#include "Player.h"
#include "Input.h"
#include "CollisionMgr.h"

Player::Player()
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
		float alpha = 1 - (i / (float)12);
		m_vInstancingColor[i] = { 1, 1, 1, alpha * alpha * alpha };
	}
	m_vInstancingColor[11] = { 1, 1, 1, 1 };

	m_pInstancingBuffer = CreateConstantBuffer(m_pd3dDevice, &m_cbInstancingData, sizeof(VS_INSTANCING_BUFFER));
	Shader* pShader;
	I_Shader.VSLoad(L"../../data/shader/VSInstancingModel.hlsl", L"VS", &pShader);
	m_pModel->SetVertexShader(pShader);
	I_Shader.PSLoad(L"../../data/shader/PSInstancingModel.hlsl", L"PS", &pShader);
	m_pModel->SetPixelShader(pShader);

	m_AttackBox.CreateOBBBox(1, 1, 1);

	m_pTrail = new TrailEffect();
	m_pTrail->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/TrailEffect.txt", L"../../data/swoosh.png");
	m_pTrail->Init();

	return true;
}

bool    Player::Frame()
{
	static float timer = 0.0f;
	timer += g_fSecondPerFrame;
	if (timer > 0.01f)
	{
		m_matInstancing.push_back(m_matWorld);
		if (m_matInstancing.size() > 11)
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

	for (int i = 0; i < 11; i++)
	{
		m_vInstancingColor[i].x = color;
		m_vInstancingColor[i].y = color;
		m_vInstancingColor[i].z = color;
	}

	Character::Frame();
	UpdateInstancingBuffer();
	m_pTrail->Frame();

	return true;
}

bool	Player::Render()
{
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &_toViewSpaceTransformBuffer);
	m_pImmediateContext->VSSetConstantBuffers(1, 1, &_objectToWorldTransformBuffer);
	m_pImmediateContext->VSSetConstantBuffers(8, 1, &m_pInstancingBuffer);
	m_pModel->RenderInstancing(12);

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

	Character::Release();

	return true;
}

void    Player::UpdateInstancingBuffer()
{
	for (int i = 0; i < m_matInstancing.size(); i++)
	{
		D3DXMatrixTranspose(&m_cbInstancingData.matWorld[10 - i], &m_matInstancing[i]);
	}
	D3DXMatrixTranspose(&m_cbInstancingData.matWorld[11], &m_matWorld);
	for (int i = 0; i < 12; i++)
	{
		m_cbInstancingData.vColor[i] = m_vInstancingColor[i];
	}

	m_pImmediateContext->UpdateSubresource(m_pInstancingBuffer, 0, nullptr, &m_cbInstancingData, 0, 0);
}