#pragma once
#include "Character.h"

class CollisionMgr : public CSingleton<CollisionMgr>
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	void	SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);

private:
	friend class CSingleton<CollisionMgr>;
	/*std::map<Character*, T_BOX*>	m_List;*/
	/*
	StaticMeshList

	NpcList
	Player < Singleton

	PlayerAttackList
	NpcAttackList
	*/

public:
	/*
	HRESULT Load(std::wstring name, Texture** retTex);
	Texture* Find(std::wstring name);
	*/
	bool	Release();

private:
	CollisionMgr();
public:
	~CollisionMgr();
};

#define I_Col CollisionMgr::GetInstance()

