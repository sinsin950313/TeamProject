#include "Emitter.h"
#include "Particle.h"
TVector3 Emitter::GetRandPos(TVector3 diff)
{
	float u = RandomStep(0.0);
	float v = RandomStep(0.0);
	float theta = u * 2.0 * XM_PI;
	float phi = acos(2.0 * v - 1.0);
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);
	float sinPhi = sin(phi);
	float cosPhi = cos(phi);

	float absX = abs(diff.x);
	float absY = abs(diff.y);
	float absZ = abs(diff.z);

	float x = RandomStep(-absX, absX) * sinPhi * cosTheta;
	float y = RandomStep(-absY, absY) * sinPhi * sinTheta;
	float z = RandomStep(-absZ, absZ) * cosPhi;
	return { x, y, z };
}

double	Emitter::GetRandRealNum(double base, double diff)
{
	double d = abs(diff);
	float min = max(base - RandomStep(0.0, d), 0);
	float max = base + RandomStep(0.0, d);
	return RandomStep(min, max);
}

TColor Emitter::GetRandColor(TColor c1, TColor c2)
{
	TColor color;

	color.R(RandomStep(c1.R(), c2.R()));
	color.G(RandomStep(c1.G(), c2.G()));
	color.B(RandomStep(c1.B(), c2.B()));
	color.A(RandomStep(c1.A(), c2.A()));

	return color;
}

void	Emitter::SpawnParticle()
{
	ParticleData data;
	// diff는 나온 값을 base에서 계산
	// 같은 값이면 0이 최소 diff의 랜덤 절대값을 랜덤으로 빼거나 더함
	// RandomStep(-diff, +diff) 가 랜덤값으로 와서 base에 더해짐 ㅇㅋ

	m_BasicData;
	m_TransData;
	for (int i = 0; i < 3; i++)
	{
		data.iTransformType[i] = m_TransData[i].iType;
		data.particleSRT[i].vFix = m_TransData[i].vFix;

		data.particleSRT[i].vPVA = m_TransData[i].vPVA + GetRandPos(m_TransData[i].vPVADiff);
		data.particleSRT[i].vVelocity = m_TransData[i].vPVAVelo + GetRandPos(m_TransData[i].vPVAVeloDiff);
		data.particleSRT[i].vCurVelocity = data.particleSRT[i].vVelocity;
		data.particleSRT[i].vAccel = m_TransData[i].vPVAAcc + GetRandPos(m_TransData[i].vPVAAccDiff);

		data.particleSRT[i].vStart = m_TransData[i].vEasingStart + GetRandPos(m_TransData[i].vEasingStartDiff);
		data.particleSRT[i].vEnd = m_TransData[i].vEasingEnd + GetRandPos(m_TransData[i].vEasingEndDiff);
	}

	data.isDestroyLifeTime = m_BasicData.isLifeTime;
	data.fLifeTime = GetRandRealNum(m_BasicData.fLifeTime, m_BasicData.fLifeTimeDiff);


	m_BasicData.iInheritPosType;
	m_BasicData.iInheritRotType;
	m_BasicData.iInheritScaleType;

	m_RenderSetData.iRenderType; // Sprite, Ring, Track 머시기 머시기

	data.ColorData.iColorType = m_RenderSetData.iColorType;

	data.ColorData.vFix = m_RenderSetData.vFixColor;

	data.ColorData.vRandom = GetRandColor(m_RenderSetData.vRandMinColor, m_RenderSetData.vRandMaxColor);

	data.ColorData.vStart = GetRandColor(m_RenderSetData.vStartColor, m_RenderSetData.vStartDiffColor);
	data.ColorData.vEnd = GetRandColor(m_RenderSetData.vEndColor, m_RenderSetData.vEndDiffColor);

	data.iBillBoardType = m_RenderSetData.iBillboard;

	if (m_pParticleList.size() < INSTNUM - 1)
	{
		Particle* pParticle = new Particle();
		pParticle->Create(data);

		TMatrix combWorld = TMatrix::Identity;
		combWorld = m_matParentScale * m_matParentRotation * m_matParentTrans;
		TVector3 vScale, vPos;
		TQuaternion q;
		combWorld.Decompose(vScale, q, vPos);

		if (m_BasicData.iInheritScaleType >= 1)
			D3DXMatrixScaling(&pParticle->m_matParentScale, vScale.x, vScale.y, vScale.z);
		if (m_BasicData.iInheritRotType >= 1)
			D3DXMatrixRotationQuaternion(&pParticle->m_matParentRotation, &q);
		if (m_BasicData.iInheritPosType >= 1)
			D3DXMatrixTranslation(&pParticle->m_matParentTrans, vPos.x, vPos.y, vPos.z);


		for (auto pChild : m_pChild)
		{
			Emitter* pEmitter = CopyEmitter(pChild);
			pParticle->m_pChildEmitterList.push_back(pEmitter);
		}

		m_pParticleList.push_back(pParticle);
	}
}

Emitter* Emitter::CopyEmitter(Emitter* pEmitter)
{
	Emitter* newEmitter = new Emitter();

	newEmitter->m_pSprite = new Sprite();
	newEmitter->m_pSprite->Create(
		m_pd3dDevice, m_pImmediateContext, L"../../data/shader/DefaultParticle.hlsl",
		pEmitter->m_BasicRenderData.texPath, L"VS", L"PS");
	newEmitter->m_pSprite->Init();

	newEmitter->Init(m_pd3dDevice, m_pImmediateContext);

	newEmitter->m_BasicData = pEmitter->m_BasicData;
	for (int i = 0; i < 3; i++)
		newEmitter->m_TransData[i] = pEmitter->m_TransData[i];

	newEmitter->m_BasicRenderData = pEmitter->m_BasicRenderData;
	newEmitter->m_RenderSetData = pEmitter->m_RenderSetData;

	newEmitter->SetCamera(m_pCamera);

	for (auto pChild : pEmitter->m_pChild)
	{
		newEmitter->m_pChild.push_back(CopyEmitter(pChild));
	}

	newEmitter->Reset();

	return newEmitter;
}

void	Emitter::Reset()
{
	m_iCurSpawn = 0;
	m_fSpawnTime = 0.0f;
	m_fCurInitTime = 0.0f;
	m_fInitTime = GetRandRealNum(m_BasicData.fInitDelay, m_BasicData.fInitDelayDiff);

	auto iter = m_pParticleList.begin();
	while (iter != m_pParticleList.end())
	{
		auto pParticle = (*iter);
		iter = m_pParticleList.erase(iter);
		pParticle->Release();
		delete pParticle;
	}
	m_pImmediateContext->UpdateSubresource(m_pInstancingBuffer, 0, nullptr, &m_cbInstancingData, 0, 0);
}

void	Emitter::SetCamera(Camera* pCamera)
{
	m_pCamera = pCamera;
}

bool	Emitter::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;

	if (!m_pSprite)
	{
		m_pSprite = new Sprite();
		std::wstring VSName = L"VS";
		std::wstring PSName = L"PS";
		m_pSprite->Create(m_pd3dDevice, m_pImmediateContext,
			L"../../data/shader/DefaultParticle.hlsl", m_BasicRenderData.texPath, VSName, PSName);

		m_pSprite->Init();
	}

	m_matParentTrans = TMatrix::Identity;
	m_matParentRotation = TMatrix::Identity;
	m_matParentScale = TMatrix::Identity;

	for (int i = 0; i < INSTNUM; i++)
	{
		m_cbInstancingData.matInst[i] = TMatrix::Identity;
		m_cbInstancingData.vColor[i] = TVector4(1, 1, 1, 1);
	}
	m_pInstancingBuffer = DX::CreateConstantBuffer(m_pd3dDevice, &m_cbInstancingData, sizeof(VS_EFFECT_BUFFER));

	m_fCurInitTime = 0.0f;
	m_fInitTime = GetRandRealNum(m_BasicData.fInitDelay, m_BasicData.fInitDelayDiff);

	m_isDone = false;

	m_BasicData.isVisible = true;
	m_BasicData.Name = L"Node";
	m_BasicData.iSpawnCount = 1;
	m_BasicData.fSpawnRate = 1.0;
	m_BasicData.fLifeTime = 1.0;
	m_BasicData.isLifeTime = true;

	m_TransData[2].vFix = TVector3(1, 1, 1);

	m_BasicRenderData.iBlendType = 2;

	m_RenderSetData.vFixColor = TColor(1, 1, 1, 1);
	m_RenderSetData.vRandMinColor = TColor(1, 1, 1, 1);
	m_RenderSetData.vRandMaxColor = TColor(1, 1, 1, 1);
	m_RenderSetData.vStartColor = TColor(1, 1, 1, 1);
	m_RenderSetData.vStartDiffColor = TColor(1, 1, 1, 1);
	m_RenderSetData.vEndColor = TColor(1, 1, 1, 1);
	m_RenderSetData.vEndDiffColor = TColor(1, 1, 1, 1);

	return true;
}
bool	Emitter::Frame()
{
	m_fCurInitTime += g_fSecondPerFrame;
	if (m_fCurInitTime < m_fInitTime)
		return true;

	m_fSpawnTime += g_fSecondPerFrame;
	if (m_fSpawnTime >= m_fSpawnTimer)
	{
		m_fSpawnTime = 0.0f;
		m_fSpawnTimer = GetRandRealNum(m_BasicData.fSpawnRate, m_BasicData.fSpawnRateDiff);

		if (m_iCurSpawn < m_BasicData.iSpawnCount || m_BasicData.isInfinite)
		{
			SpawnParticle();
			m_iCurSpawn++;
		}
		else
		{
			m_isDone = true;
		}
	}

	ParticleFrame();
	m_pSprite->Frame();

	return true;
}
void	Emitter::ParticleFrame()
{
	int i = 0;
	auto iter = m_pParticleList.begin();
	while (iter != m_pParticleList.end())
	{
		if ((*iter)->m_isDone)
		{
			auto pParticle = (*iter);
			iter = m_pParticleList.erase(iter);
			pParticle->Release();
			delete pParticle;
		}
		else
		{
			if (m_BasicData.iInheritScaleType == 2)
				D3DXMatrixMultiply(&(*iter)->m_matParentScale, &TMatrix::Identity, &m_matParentScale);
			if (m_BasicData.iInheritRotType == 2)
				D3DXMatrixMultiply(&(*iter)->m_matParentRotation, &TMatrix::Identity, &m_matParentRotation);
			if (m_BasicData.iInheritPosType == 2)
				D3DXMatrixMultiply(&(*iter)->m_matParentTrans, &TMatrix::Identity, &m_matParentTrans);

			(*iter)->Frame(&m_pCamera->m_matView);

			D3DXMatrixTranspose(&m_cbInstancingData.matInst[i], &(*iter)->m_matWorld);
			m_cbInstancingData.vColor[i] = (*iter)->m_vCurColor.ToVector4();

			for (int v = 0; v < 2; v++)
			{
				TVector3 p;
				D3DXVec3TransformCoord(&p, &m_pSprite->m_VertexList[2 * v + 0].p, &(*iter)->m_matWorld);
				TVector3 p1;
				D3DXVec3TransformCoord(&p1, &m_pSprite->m_VertexList[2 * v + 1].p, &(*iter)->m_matWorld);
				TVector4 v1 = TVector4(p.x, p.y, p.z, 1);
				TVector4 v2 = TVector4(p1.x, p1.y, p1.z, 1);
				D3DXVec4Lerp(&m_cbInstancingData.vCalc[i][v], &v1, &v2, 0.5);
				float u = (i + 1) / ((float)m_pParticleList.size());
				m_cbInstancingData.vUV[i][v] = TVector4(v, u, 0, 0);

				m_cbInstancingData.vCalc[i + 1][v] = m_cbInstancingData.vCalc[i][v];
				m_cbInstancingData.vUV[i + 1][v] = m_cbInstancingData.vUV[i][v];
			}
			m_cbInstancingData.matInst[i + 1] = m_cbInstancingData.matInst[i];
			m_cbInstancingData.vColor[i + 1] = m_cbInstancingData.vColor[i];
			i++;
			iter++;
		}
	}

	m_pImmediateContext->UpdateSubresource(m_pInstancingBuffer, 0, nullptr, &m_cbInstancingData, 0, 0);
}
#include "DXState.h"
bool	Emitter::Render()
{
	if (m_BasicData.isVisible)
	{
		m_pSprite->m_pVS = m_pSprite->m_pSwapVS[0];
		m_pSprite->m_pGS = m_pSprite->m_pSwapGS[0];
		m_pSprite->m_pPS = m_pSprite->m_pSwapPS[0];

		if (m_BasicRenderData.texPath == L"")
			m_pSprite->m_pPS = m_pSprite->m_pSwapPS[2];

		if (m_BasicRenderData.iBlendType == 0)
			m_pImmediateContext->OMSetBlendState(DXState::g_pAlphaBlend, 0, -1);
		else
			m_pImmediateContext->OMSetBlendState(DXState::g_pAddAlphaBlend, 0, -1);

		if (m_BasicRenderData.iMaterial == 1)
			m_pSprite->m_pPS = m_pSprite->m_pSwapPS[1];


		if (m_RenderSetData.iRenderType == 1)
		{
			m_pSprite->m_pVS = m_pSprite->m_pSwapVS[1];
			m_pSprite->m_pGS = m_pSprite->m_pSwapGS[1];
			m_pImmediateContext->GSSetConstantBuffers(0, 1, &m_pSprite->m_pConstantBuffer);
			m_pImmediateContext->GSSetConstantBuffers(8, 1, &m_pInstancingBuffer);
		}

		m_pImmediateContext->OMSetDepthStencilState(DXState::g_pDefaultDepthStencilAndNoWrite, 0xff);

		m_pSprite->SetMatrix(nullptr, &m_pCamera->m_matView, &m_pCamera->m_matProj);
		m_pImmediateContext->VSSetConstantBuffers(8, 1, &m_pInstancingBuffer);
		m_pSprite->RenderInstancing(m_pParticleList.size());

		m_pImmediateContext->OMSetBlendState(DXState::g_pAlphaBlend, 0, -1);
		m_pImmediateContext->OMSetDepthStencilState(DXState::g_pDefaultDepthStencil, 0xff);
	}
	for (auto pChild : m_pParticleList)
	{
		pChild->Render();
	}

	return true;
}
bool	Emitter::Release()
{
	for (auto pChild : m_pChild)
	{
		pChild->Release();
		delete pChild;
	}

	for (auto particle : m_pParticleList)
	{
		particle->Release();
		delete particle;
	}
	m_pParticleList.clear();

	if (m_pInstancingBuffer)
	{
		m_pInstancingBuffer->Release();
		m_pInstancingBuffer = nullptr;
	}

	if (m_pSprite)
	{
		m_pSprite->Release();
		delete m_pSprite;
		m_pSprite = nullptr;
	}
	return true;
}