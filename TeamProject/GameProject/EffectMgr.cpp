#include "EffectMgr.h"
#include <fstream>
#include <sstream>

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
	FILE* stream;	
	if (_wfopen_s(&stream, path.c_str(), L"rt, ccs=UNICODE") != 0)
	{
		return;
	}
	std::wifstream is(stream);
	std::wstring str;

	auto LoadBasicData = [&](BasicData* bd)
	{
		std::getline(is, str);
		bd->isVisible = _ttoi64(str.c_str());

		std::getline(is, str);
		bd->Name = str.c_str();

		std::getline(is, str);
		bd->iSpawnCount = _ttoi64(str.c_str());

		std::getline(is, str);
		bd->isInfinite = _ttoi64(str.c_str());

		std::getline(is, str);
		bd->iInheritPosType = _ttoi64(str.c_str());

		std::getline(is, str);
		bd->iInheritRotType = _ttoi64(str.c_str());

		std::getline(is, str);
		bd->iInheritScaleType = _ttoi64(str.c_str());

		std::getline(is, str);
		bd->isLifeTime = _ttoi64(str.c_str());

		std::getline(is, str);
		bd->fLifeTime = _ttof(str.c_str());

		std::getline(is, str);
		bd->fLifeTimeDiff = _ttof(str.c_str());

		std::getline(is, str);
		bd->fSpawnRate = _ttof(str.c_str());

		std::getline(is, str);
		bd->fSpawnRateDiff = _ttof(str.c_str());

		std::getline(is, str);
		bd->fInitDelay = _ttof(str.c_str());

		std::getline(is, str);
		bd->fInitDelayDiff = _ttof(str.c_str());
	};

	auto LoadTransformData = [&](TransformData* td)
	{
		std::getline(is, str);
		td->iType = _ttoi64(str.c_str());

		auto LoadVector3 = [&](TVector3* v)
		{
			std::getline(is, str);
			v->x = _ttof(str.c_str());
			std::getline(is, str);
			v->y = _ttof(str.c_str());
			std::getline(is, str);
			v->z = _ttof(str.c_str());
		};

		td->vFix;
		LoadVector3(&td->vFix);

		td->vPVA;
		LoadVector3(&td->vPVA);
		td->vPVADiff;
		LoadVector3(&td->vPVADiff);
		td->vPVAVelo;
		LoadVector3(&td->vPVAVelo);
		td->vPVAVeloDiff;
		LoadVector3(&td->vPVAVeloDiff);
		td->vPVAAcc;
		LoadVector3(&td->vPVAAcc);
		td->vPVAAccDiff;
		LoadVector3(&td->vPVAAccDiff);

		td->vEasingStart;
		LoadVector3(&td->vEasingStart);
		td->vEasingStartDiff;
		LoadVector3(&td->vEasingStartDiff);
		td->vEasingEnd;
		LoadVector3(&td->vEasingEnd);
		td->vEasingEndDiff;
		LoadVector3(&td->vEasingEndDiff);
	};

	auto LoadBasicRenderData = [&](BasicRenderData* brd)
	{
		std::getline(is, str);
		brd->iMaterial = _ttoi64(str.c_str());

		std::getline(is, str);
		brd->texPath = str.c_str();

		std::getline(is, str);
		brd->iBlendType = _ttoi64(str.c_str());
	};

	auto LoadRenderSetData = [&](RenderSetData* rsd)
	{
		std::getline(is, str);
		rsd->iRenderType = _ttoi64(str.c_str());

		std::getline(is, str);
		rsd->iColorType = _ttoi64(str.c_str());

		auto LoadTColor = [&](TColor* c)
		{
			std::getline(is, str);
			c->R(_ttof(str.c_str()));
			std::getline(is, str);
			c->G(_ttof(str.c_str()));
			std::getline(is, str);
			c->B(_ttof(str.c_str()));
			std::getline(is, str);
			c->A(_ttof(str.c_str()));
		};

		rsd->vFixColor;
		LoadTColor(&rsd->vFixColor);

		rsd->vRandMinColor;
		LoadTColor(&rsd->vRandMinColor);
		rsd->vRandMaxColor;
		LoadTColor(&rsd->vRandMaxColor);

		rsd->vStartColor;
		LoadTColor(&rsd->vStartColor);
		rsd->vStartDiffColor;
		LoadTColor(&rsd->vStartDiffColor);
		rsd->vEndColor;
		LoadTColor(&rsd->vEndColor);
		rsd->vEndDiffColor;
		LoadTColor(&rsd->vEndDiffColor);

		std::getline(is, str);
		rsd->iBillboard = _ttoi64(str.c_str());
	};

	auto LoadEmitter = [&](Emitter* pEmitter)
	{
		std::getline(is, str);
		size_t iEmitterNum = _ttoi64(str.c_str());
		for (int i = 0; i < iEmitterNum; i++)
		{
			Emitter* pChild = new Emitter();
			pChild->Init(pEmitter->m_pd3dDevice, pEmitter->m_pImmediateContext);
			pEmitter->m_pChild.push_back(pChild);
		}

		//pEmitter->m_BasicData;
		LoadBasicData(&pEmitter->m_BasicData);

		//pEmitter->m_TransData[3];
		LoadTransformData(&pEmitter->m_TransData[0]);
		LoadTransformData(&pEmitter->m_TransData[1]);
		LoadTransformData(&pEmitter->m_TransData[2]);

		//pEmitter->m_BasicRenderData;
		LoadBasicRenderData(&pEmitter->m_BasicRenderData);

		//pEmitter->m_RenderSetData;
		LoadRenderSetData(&pEmitter->m_RenderSetData);

		pEmitter->m_pSprite = new Sprite();

		std::wstring PSName = L"PS";
		if (pEmitter->m_BasicRenderData.iMaterial == 1)
			PSName = L"Distortion";
		pEmitter->m_pSprite->Create(
			m_pd3dDevice, m_pImmediateContext, L"../../data/shader/DefaultParticle.txt", pEmitter->m_BasicRenderData.texPath, L"VS", PSName);
		pEmitter->m_pSprite->Init();
	};

	auto ProcessLoad = [&](Emitter* pEmitter)
	{
		LoadEmitter(pEmitter);

		for (auto pChild : pEmitter->m_pChild)
		{
			LoadEmitter(pChild);
		}
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////
	Effect* pEffect = new Effect();
	pEffect->Init(m_pd3dDevice, m_pImmediateContext);
	pEffect->m_vPos = vPos;

	std::getline(is, str);
	DWORD iEmitterNum = _ttoi64(str.c_str());
	if (iEmitterNum == 0)
	{
		pEffect->Release();
		delete pEffect;
		is.close();
		return;
	}
	for (int i = 0; i < iEmitterNum; i++)
	{
		Emitter* pEmitter = new Emitter();
		pEmitter->Init(pEffect->m_pd3dDevice, pEffect->m_pImmediateContext);
		pEffect->m_pEmitterList.push_back(pEmitter);

		ProcessLoad(pEmitter);
	}
	m_List.push_back(pEffect);

	is.close();
}

bool	EffectMgr::Frame()
{
	auto iter = m_List.begin();
	while (iter != m_List.end())
	{
		auto pEffect = (*iter);
		if (pEffect->m_isDone)
		{
			iter = m_List.erase(iter);
			pEffect->Release();
			delete pEffect;
		}
		else
		{
			pEffect->SetCamera(m_pCamera);
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
	Release();
}