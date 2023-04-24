#include "Emitter.h"

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

	if (m_pParticleList.size() < 500)
	{
		Particle* pParticle = new Particle();
		pParticle->Create(data);
		m_pParticleList.push_back(pParticle);
	}
}

void	Emitter::CalcInheritMatrix(Emitter* pChild)
{
	auto GetVector = [&](TransformData data)->TVector3 {
		switch (data.iType)
		{
		case 0:
			return data.vFix;
			break;
		case 1:
			return data.vPVA;
			break;
		case 2:
			return data.vEasingStart;
			break;
		}
	};

	m_BasicData.iInheritScaleType;
	m_BasicData.iInheritRotType;
	m_BasicData.iInheritPosType;
	TVector3 scale = GetVector(m_TransData[0]);
	TVector3 rot = GetVector(m_TransData[1]);
	TVector3 pos = GetVector(m_TransData[2]);
	
	TMatrix matScale;
	D3DXMatrixScaling(&matScale, scale.x, scale.y, scale.z);

	TQuaternion qRotation;
	D3DXQuaternionRotationYawPitchRoll(&qRotation, rot.y, rot.x, rot.z);

	TMatrix matTranslate;
	D3DXMatrixTranslation(&matTranslate, pos.x, pos.y, pos.z);

	if (pChild->m_BasicData.iInheritScaleType > 0)
		D3DXMatrixMultiply(&pChild->m_matParentScale, &TMatrix::Identity, &matScale);
	if (pChild->m_BasicData.iInheritRotType > 0)
		D3DXMatrixRotationQuaternion(&pChild->m_matParentRotation, &qRotation);
	if (pChild->m_BasicData.iInheritPosType > 0)
		D3DXMatrixMultiply(&pChild->m_matParentTrans, &TMatrix::Identity, &matTranslate);
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
		delete pParticle;
	}
	for (int i = 0; i < 500; i++)
	{
		m_cbInstancingData.matInst[i] = TMatrix::Identity;
		m_cbInstancingData.vColor[i] = TVector4(1, 1, 1, 1);
	}
	m_pImmediateContext->UpdateSubresource(m_pInstancingBuffer, 0, nullptr, &m_cbInstancingData, 0, 0);

	for (auto pChild : m_pChild)
	{
		pChild->Reset();
	}
}

void	Emitter::SetCamera(Camera* pCamera)
{
	for (auto pChild : m_pChild)
	{
		pChild->SetCamera(pCamera);
	}
	m_pCamera = pCamera;
}

bool	Emitter::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;

	m_matParentTrans = TMatrix::Identity;
	m_matParentRotation = TMatrix::Identity;
	m_matParentScale = TMatrix::Identity;

	for (int i = 0; i < 500; i++)
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
			//m_isDone = true;
		}
	}

	ParticleFrame();
	m_pSprite->Frame();

	for (auto pChild : m_pChild)
	{
		// Child Emitter의 프레임이나 여타 함수에 넘겨야할듯
		// 여기서 부모 행렬을 들고 프레임 들어가서 받아온 값으로 파티클 생성
		// 
		//(*m_pParticleList.begin())->
		CalcInheritMatrix(pChild);
		pChild->Frame();
	}

	return true;
}
void	Emitter::ParticleFrame()
{
	int i = 0;
	auto iter = m_pParticleList.begin();
	while (iter != m_pParticleList.end())
	{
		(*iter)->Frame(&m_pCamera->m_matView);
		if ((*iter)->m_isDone)
		{
			auto pParticle = (*iter);
			iter = m_pParticleList.erase(iter);
			delete pParticle;
		}
		else
		{
			TMatrix matWorld;
			D3DXMatrixMultiply(&matWorld, &(*iter)->m_matWorld, &m_matParentScale);
			D3DXMatrixMultiply(&matWorld, &matWorld, &m_matParentRotation);
			D3DXMatrixMultiply(&matWorld, &matWorld, &m_matParentTrans);
			D3DXMatrixTranspose(&m_cbInstancingData.matInst[i], &matWorld);
			m_cbInstancingData.vColor[i] = (*iter)->m_vCurColor.ToVector4();
			i++;
			iter++;
		}
	}
	m_pImmediateContext->UpdateSubresource(m_pInstancingBuffer, 0, nullptr, &m_cbInstancingData, 0, 0);
}
#include "DXState.h"
bool	Emitter::Render()
{
	if (!m_BasicData.isVisible)
		return true;
	// 랜더 타겟을 사용해서 기존 랜더타겟 쉐이더 리소스뷰를 복사해서 넣어서 랜더링 후 나온 쉐이더 리소스 뷰를
	if(m_BasicRenderData.iBlendType == 0)
		m_pImmediateContext->OMSetBlendState(DXState::g_pAlphaBlend, 0, -1);
	else
		m_pImmediateContext->OMSetBlendState(DXState::g_pAddAlphaBlend, 0, -1);

	m_pImmediateContext->OMSetDepthStencilState(DXState::g_pDefaultDepthStencilAndNoWrite, 0xff);

	m_pSprite->SetMatrix(nullptr, &m_pCamera->m_matView, &m_pCamera->m_matProj);
	m_pImmediateContext->VSSetConstantBuffers(8, 1, &m_pInstancingBuffer);
	m_pSprite->RenderInstancing(m_pParticleList.size());

	m_pImmediateContext->OMSetBlendState(DXState::g_pAlphaBlend, 0, -1);
	m_pImmediateContext->OMSetDepthStencilState(DXState::g_pDefaultDepthStencil, 0xff);

	for (auto pChild : m_pChild)
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