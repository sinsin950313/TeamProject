#pragma once
#include "Sprite.h"

class SpriteMgr : public Singleton<SpriteMgr>
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	void	SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);

private:
	friend class Singleton<SpriteMgr>;
	std::map<std::wstring, Sprite*>	m_List;

public:
	HRESULT Load(std::wstring name, Sprite** retSprite);
	Sprite* Find(std::wstring name);
	bool	Release();

private:
	SpriteMgr();
public:
	~SpriteMgr();
};

#define I_Sprite SpriteMgr::GetInstance()

