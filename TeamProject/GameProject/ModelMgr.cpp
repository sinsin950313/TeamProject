#include "ModelMgr.h"
#include "FileIOObject.h"

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

	// 중복된 Model을 불러오더라도 다시 new처리를 하므로 Memory Leak을 유발합니다
	// 일단은 그냥 Character에서 메모리를 해제하도록 수정해놨습니다.
	//SSB::Model* pModel = Find(name);
	//if (pModel != nullptr)
	//{
	//	*retModel = pModel;
	//	return S_OK;
	//}

	SSB::Model* pModel = new SSB::Model();
	if (pModel)
	{
        SSB::ObjectScriptIO io;
        std::string filename = name;
        auto scriptInfo = io.Read(filename);

		// 로드 실패가 정의되지 않는 방식
		pModel->SetDevice(m_pd3dDevice, m_pImmediateContext);
		int offset = 0;
		pModel->Deserialize(scriptInfo.Pointer, scriptInfo.BufferSize, offset);
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

HRESULT ModelMgr::Load(std::string filename, std::string name, std::string anim, SSB::Model** retModel)
{
	if (retModel)
	{
		*retModel = nullptr;
	}
	else if (!retModel)
	{
		return E_INVALIDARG;
	}

	// 중복된 Model을 불러오더라도 다시 new처리를 하므로 Memory Leak을 유발합니다
	// 일단은 그냥 Character에서 메모리를 해제하도록 수정해놨습니다.
	//SSB::Model* pModel = Find(filename);
	//if (pModel != nullptr)
	//{
	//	*retModel = pModel;
	//	return S_OK;
	//}

	SSB::Model* pModel = new SSB::Model();
	if (pModel)
	{
        SSB::ObjectScriptIO io;
        std::string filename = name;
        auto scriptInfo = io.Read(filename);

		// 로드 실패가 정의되지 않는 방식
		pModel->SetDevice(m_pd3dDevice, m_pImmediateContext);
		int offset = 0;
		pModel->Deserialize(scriptInfo.Pointer, scriptInfo.BufferSize, offset);
		pModel->Init();
		pModel->SetCurrentAnimation(anim);
		m_List.insert(std::make_pair(filename, pModel));
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
	//for (std::pair<std::string, SSB::Model*> data : m_List)
	//{
	//	SSB::Model* pModel = data.second;
	//	if (pModel) pModel->Release();
	//	delete pModel;
	//	pModel = nullptr;
	//}
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