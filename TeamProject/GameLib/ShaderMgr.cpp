#include "ShaderMgr.h"

void	ShaderMgr::SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;
}

HRESULT ShaderMgr::Load(std::wstring name, Shader** retShader)
{
	return Load(name, L"VS", L"PS", retShader);
}

HRESULT ShaderMgr::Load(std::wstring name, std::wstring VSname, std::wstring PSname, Shader** retShader)
{
	HRESULT hr;

	if (retShader)
	{
		*retShader = nullptr;
	}
	else if (!retShader)
	{
		return E_INVALIDARG;
	}
	Shader* pShader = Find(name + VSname + PSname);
	if (pShader != nullptr)
	{
		*retShader = pShader;
		return S_OK;
	}

	pShader = new Shader;
	if (pShader)
	{
		hr = pShader->Load(m_pd3dDevice, m_pImmediateContext, name, VSname, PSname);
		if (SUCCEEDED(hr))
		{
			std::wstring LastName = name + VSname + PSname;
			m_List.insert(std::make_pair(LastName, pShader));
			*retShader = pShader;
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT ShaderMgr::VSLoad(std::wstring name, std::wstring VSfunc, Shader** retShader)
{
	HRESULT hr;

	if (retShader)
	{
		*retShader = nullptr;
	}
	else if (!retShader)
	{
		return E_INVALIDARG;
	}
	Shader* pShader = Find(name + VSfunc);
	if (pShader != nullptr)
	{
		*retShader = pShader;
		return S_OK;
	}

	pShader = new Shader;
	if (pShader)
	{
		hr = pShader->VSLoad(m_pd3dDevice, m_pImmediateContext, name, VSfunc);
		if (SUCCEEDED(hr))
		{
			std::wstring LastName = name + VSfunc;
			m_List.insert(std::make_pair(LastName, pShader));
			*retShader = pShader;
			return S_OK;
		}
	}
	return E_FAIL;
}


HRESULT ShaderMgr::GSLoad(std::wstring name, std::wstring GSfunc, Shader** retShader)
{
	HRESULT hr;

	if (retShader)
	{
		*retShader = nullptr;
	}
	else if (!retShader)
	{
		return E_INVALIDARG;
	}
	Shader* pShader = Find(name + GSfunc);
	if (pShader != nullptr)
	{
		*retShader = pShader;
		return S_OK;
	}

	pShader = new Shader;
	if (pShader)
	{
		hr = pShader->GSLoad(m_pd3dDevice, m_pImmediateContext, name, GSfunc);
		if (SUCCEEDED(hr))
		{
			std::wstring LastName = name + GSfunc;
			m_List.insert(std::make_pair(LastName, pShader));
			*retShader = pShader;
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT ShaderMgr::PSLoad(std::wstring name, std::wstring PSfunc, Shader** retShader)
{
	HRESULT hr;

	if (retShader)
	{
		*retShader = nullptr;
	}
	else if (!retShader)
	{
		return E_INVALIDARG;
	}
	Shader* pShader = Find(name + PSfunc);
	if (pShader != nullptr)
	{
		*retShader = pShader;
		return S_OK;
	}

	pShader = new Shader;
	if (pShader)
	{
		hr = pShader->PSLoad(m_pd3dDevice, m_pImmediateContext, name, PSfunc);
		if (SUCCEEDED(hr))
		{
			std::wstring LastName = name + PSfunc;
			m_List.insert(std::make_pair(LastName, pShader));
			*retShader = pShader;
			return S_OK;
		}
	}
	return E_FAIL;
}

Shader* ShaderMgr::Find(std::wstring name)
{
	std::map<std::wstring, Shader*>::iterator iter = m_List.find(name);
	if (iter != m_List.end())
	{
		return iter->second;
	}
	return nullptr;
}

bool	ShaderMgr::Release()
{
	for (std::pair<std::wstring, Shader*> data : m_List)
	{
		Shader* pShader = data.second;
		if (pShader) pShader->Release();
		delete pShader;
		pShader = nullptr;
	}
	m_List.clear();
	return true;
}

ShaderMgr::ShaderMgr()
{

}

ShaderMgr::~ShaderMgr()
{
	Release();
}