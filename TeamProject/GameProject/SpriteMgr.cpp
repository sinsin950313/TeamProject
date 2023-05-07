#include "SpriteMgr.h"

void	SpriteMgr::SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;
}

HRESULT SpriteMgr::Load(std::wstring name, Sprite** retSprite)
{
	if (retSprite)
	{
		*retSprite = nullptr;
	}
	else if (!retSprite)
	{
		return E_INVALIDARG;
	}

	Sprite* pSprite = Find(name);
	if (pSprite != nullptr)
	{
		*retSprite = pSprite;
		return S_OK;
	}

	pSprite = new Sprite();

	pSprite->BaseObject::CreateVertexData();
	pSprite->BaseObject::CreateIndexData();
	std::wstring VSName = L"VS";
	std::wstring PSName = L"PS";
	if (pSprite)
	{
		if (pSprite->Create(m_pd3dDevice, m_pImmediateContext,
			L"../../data/shader/DefaultParticle.hlsl", name, VSName, PSName))
		{
			pSprite->Init();
			m_List.insert(std::make_pair(name, pSprite));
			*retSprite = pSprite;
			return S_OK;
		}
		else
		{
			delete pSprite;
			*retSprite = nullptr;
			return E_FAIL;
		}
	}
	return E_FAIL;
}

Sprite* SpriteMgr::Find(std::wstring name)
{
	std::map<std::wstring, Sprite*>::iterator iter = m_List.find(name);
	if (iter != m_List.end())
	{
		return iter->second;
	}
	return nullptr;
}

bool	SpriteMgr::Release()
{
	for (auto iter : m_List)
	{
		Sprite* pSprite = iter.second;
		pSprite->Release();
		delete pSprite;
		pSprite = nullptr;
	}
	m_List.clear();
	return true;
}

SpriteMgr::SpriteMgr()
{

}
SpriteMgr::~SpriteMgr()
{
	Release();
}