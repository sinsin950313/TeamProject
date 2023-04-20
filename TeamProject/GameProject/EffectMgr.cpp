#include "EffectMgr.h"

void	EffectMgr::SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;
}

void	EffectMgr::SetCamera(Camera* pCamera)
{
	m_pCamera = pCamera;
}

void	EffectMgr::CreateEffect(std::wstring path, TVector3 vPos)
{
	Effect* pEffect = new Effect();

	pEffect->m_vPos = vPos;

	Emitter* pEmitter = new Emitter;

	Sprite* pSprite = new Sprite();

	pSprite->CreateVertexData();
	pSprite->CreateIndexData();
	pSprite->Create(
		m_pd3dDevice, m_pImmediateContext, L"../../data/shader/DefaultParticle.txt", L"../../data/effectdata/Particle01.png");
	pSprite->Init();

	pEmitter->m_pSprite = pSprite;
	pEmitter->Init(m_pd3dDevice, m_pImmediateContext);
	pEmitter->SetCamera(m_pCamera);


	pEmitter->m_BasicData.isVisible = true;
	pEmitter->m_BasicData.iSpawnCount = 1;
	pEmitter->m_BasicData.isInfinite = true;
	pEmitter->m_BasicData.iInheritPosType = 0;
	pEmitter->m_BasicData.iInheritRotType = 0;
	pEmitter->m_BasicData.iInheritScaleType = 0;
	pEmitter->m_BasicData.isLifeTime = true;
	pEmitter->m_BasicData.fLifeTime = 1.0f;
	pEmitter->m_BasicData.fLifeTimeDiff = 0.5f;
	pEmitter->m_BasicData.fSpawnRate = 0.05f;
	pEmitter->m_BasicData.fSpawnRateDiff = 0.0f;
	pEmitter->m_BasicData.fInitDelay = 0.0f;
	pEmitter->m_BasicData.fInitDelayDiff = 0.0f;

	pEmitter->m_TransData[0].iType = 1;
	pEmitter->m_TransData[0].vFix = TVector3(0, 0, 0);
	pEmitter->m_TransData[0].vPVA = TVector3(0, 0, 0);
	pEmitter->m_TransData[0].vPVADiff = TVector3(3, 3, 3);
	pEmitter->m_TransData[0].vPVAVelo = TVector3(0.02, 0.02, 0.02);
	pEmitter->m_TransData[0].vPVAVeloDiff = TVector3(0.01, 0.01, 0.01);
	pEmitter->m_TransData[0].vPVAAcc = TVector3(0, 0, 0);
	pEmitter->m_TransData[0].vPVAAccDiff = TVector3(0, 0, 0);
	pEmitter->m_TransData[0].vEasingStart = TVector3(0, 0, 0);
	pEmitter->m_TransData[0].vEasingStartDiff = TVector3(0, 0, 0);
	pEmitter->m_TransData[0].vEasingEnd = TVector3(0, 0, 0);
	pEmitter->m_TransData[0].vEasingEndDiff = TVector3(0, 0, 0);

	pEmitter->m_TransData[1].iType = 0;
	pEmitter->m_TransData[1].vFix = TVector3(0, 0, 0);
	pEmitter->m_TransData[1].vPVA = TVector3(0, 0, 0);
	pEmitter->m_TransData[1].vPVADiff = TVector3(3, 3, 3);
	pEmitter->m_TransData[1].vPVAVelo = TVector3(0.02, 0.02, 0.02);
	pEmitter->m_TransData[1].vPVAVeloDiff = TVector3(0.01, 0.01, 0.01);
	pEmitter->m_TransData[1].vPVAAcc = TVector3(0, 0, 0);
	pEmitter->m_TransData[1].vPVAAccDiff = TVector3(0, 0, 0);
	pEmitter->m_TransData[1].vEasingStart = TVector3(0, 0, 0);
	pEmitter->m_TransData[1].vEasingStartDiff = TVector3(0, 0, 0);
	pEmitter->m_TransData[1].vEasingEnd = TVector3(0, 0, 0);
	pEmitter->m_TransData[1].vEasingEndDiff = TVector3(0, 0, 0);

	pEmitter->m_TransData[2].iType = 2;
	pEmitter->m_TransData[2].vFix = TVector3(0, 0, 0);
	pEmitter->m_TransData[2].vPVA = TVector3(0, 0, 0);
	pEmitter->m_TransData[2].vPVADiff = TVector3(3, 3, 3);
	pEmitter->m_TransData[2].vPVAVelo = TVector3(0.02, 0.02, 0.02);
	pEmitter->m_TransData[2].vPVAVeloDiff = TVector3(0.01, 0.01, 0.01);
	pEmitter->m_TransData[2].vPVAAcc = TVector3(0, 0, 0);
	pEmitter->m_TransData[2].vPVAAccDiff = TVector3(0, 0, 0);
	pEmitter->m_TransData[2].vEasingStart = TVector3(2, 2, 2);
	pEmitter->m_TransData[2].vEasingStartDiff = TVector3(1, 1, 1);
	pEmitter->m_TransData[2].vEasingEnd = TVector3(0, 0, 0);
	pEmitter->m_TransData[2].vEasingEndDiff = TVector3(0, 0, 0);

	pEffect->m_pEmitterList.push_back(pEmitter);
	m_List.push_back(pEffect);
}

bool	EffectMgr::Frame()
{
	auto iter = m_List.begin();
	while (iter != m_List.end())
	{
		auto pEffect = (*iter);
		if (false)
		{
			iter = m_List.erase(iter);
			pEffect->Release();
			delete pEffect;
		}
		else
		{
			pEffect->Frame();
			iter++;
		}
	}
	return true;
}

bool	EffectMgr::Render()
{
	for (auto pEffect : m_List)
	{
		pEffect->Render();
	}
	return true;
}

bool	EffectMgr::Release()
{
	auto iter = m_List.begin();
	while (iter != m_List.end())
	{
		auto pEffect = (*iter);
		iter = m_List.erase(iter);
		pEffect->Release();
		delete pEffect;
	}
	m_List.clear();

	return true;
}

std::wstring EffectMgr::ChangePath(std::wstring fullpath)
{
	return L"";
}

EffectMgr::EffectMgr()
{

}

EffectMgr::~EffectMgr()
{

}