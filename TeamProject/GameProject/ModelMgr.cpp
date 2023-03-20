#include "ModelMgr.h"

void	ModelMgr::SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;
}

HRESULT ModelMgr::Load(std::string name, std::string anim, SSB::Model** retModel)
{
	if (retModel)
	{
		*retModel = nullptr;
	}
	else if (!retModel)
	{
		return E_INVALIDARG;
	}

	SSB::Model* pModel = Find(name);
	if (pModel != nullptr)
	{
		*retModel = pModel;
		return S_OK;
	}

	pModel = new SSB::Model();
	if (pModel)
	{
		// 로드 실패가 정의되지 않는 방식
		pModel->SetDevice(m_pd3dDevice, m_pImmediateContext);
		pModel->Deserialize(name);
		pModel->Init();
		pModel->SetCurrentAnimation(anim);
		m_List.insert(std::make_pair(name, pModel));
		*retModel = pModel;
		return S_OK;

		/*
		if (pModel->Load(m_pd3dDevice, m_pImmediateContext, name))
		{
			m_List.insert(std::make_pair(name, pModel));
			*retTex = pModel;
			return S_OK;
		}
		else
		{
			delete pModel;
			*retTex = nullptr;
			return E_FAIL;
		}
		*/
	}
	return E_FAIL;
}

SSB::Model* ModelMgr::Find(std::string name)
{
	std::map<std::string, SSB::Model*>::iterator iter = m_List.find(name);
	if (iter != m_List.end())
	{
		return iter->second;
	}
	return nullptr;
}

bool	ModelMgr::Release()
{
	for (std::pair<std::string, SSB::Model*> data : m_List)
	{
		SSB::Model* pModel = data.second;
		if (pModel) pModel->Release();
		delete pModel;
		pModel = nullptr;
	}
	m_List.clear();
	return true;
}

ModelMgr::ModelMgr()
{

}

ModelMgr::~ModelMgr()
{
	Release();
}