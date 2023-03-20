#pragma once
#include "Texture.h"
class TextureMgr : public Singleton<TextureMgr>
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	void	SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);

private:
	friend class Singleton<TextureMgr>;
	std::map<std::wstring, Texture*>	m_List;

public:
	HRESULT Load(std::wstring name, Texture** retTex);
	Texture* Find(std::wstring name);
	bool	Release();
	std::wstring GetSplitName(std::wstring fullpath);
	std::wstring GetSplitName(std::string fullpath);
private:
	TextureMgr();
public:
	~TextureMgr();
};

#define I_Tex TextureMgr::GetInstance()
