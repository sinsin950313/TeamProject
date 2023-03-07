#include "TextureMgr.h"

void	TextureMgr::SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;
}

std::wstring TextureMgr::GetSplitName(std::wstring fullpath)
{
	std::wstring name;
	TCHAR drive[MAX_PATH] = { 0, };
	TCHAR dir[MAX_PATH] = { 0, };
	TCHAR filename[MAX_PATH] = { 0, };
	TCHAR ext[MAX_PATH] = { 0, };
	_tsplitpath_s(fullpath.c_str(), drive, dir, filename, ext);
	name = filename;
	name += ext;
	return name;
}
std::wstring TextureMgr::GetSplitName(std::string fullpath)
{
	std::wstring szUnicode = to_mw(fullpath);
	std::wstring name;
	TCHAR drive[MAX_PATH] = { 0, };
	TCHAR dir[MAX_PATH] = { 0, };
	TCHAR filename[MAX_PATH] = { 0, };
	TCHAR ext[MAX_PATH] = { 0, };
	_tsplitpath_s(szUnicode.c_str(), drive, dir, filename, ext);
	if (_tcsicmp(ext, _T(".tga")) == 0 || _tcsicmp(ext, _T(".TGA")) == 0)
	{
		_tcscpy_s(ext, _T(".dds"));
	}
	name = filename;
	name += ext;
	return name;
}

HRESULT TextureMgr::Load(std::wstring name, Texture** retTex)
{
	if (retTex)
	{
		*retTex = nullptr;
	}
	else if (!retTex)
	{
		return E_INVALIDARG;
	}

	Texture* pTex = Find(name);
	if (pTex != nullptr) 
	{
		*retTex = pTex;
		return S_OK;
	}

	pTex = new Texture;
	if (pTex)
	{
		if (pTex->Load(m_pd3dDevice, m_pImmediateContext, name))
		{
			m_List.insert(std::make_pair(name, pTex));
			*retTex = pTex;
			return S_OK;
		}
		else
		{
			delete pTex;
			*retTex = nullptr;
			return E_FAIL;
		}
	}
	return E_FAIL;
}

Texture* TextureMgr::Find(std::wstring name)
{
	std::map<std::wstring, Texture*>::iterator iter = m_List.find(name);
	if (iter != m_List.end())
	{
		return iter->second;
	}
	return nullptr;
}

bool	TextureMgr::Release()
{
	for (std::pair<std::wstring, Texture*> data : m_List)
	{
		Texture* pTex = data.second;
		if (pTex) pTex->Release();
		delete pTex;
		pTex = nullptr;
	}
	m_List.clear();
	return true;
}

TextureMgr::TextureMgr()
{

}

TextureMgr::~TextureMgr()
{
	Release();
}