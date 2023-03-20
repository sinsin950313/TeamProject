#pragma once
#include "Model.h"

class ModelMgr : public Singleton<ModelMgr>
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	void	SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);

private:
	friend class Singleton<ModelMgr>;
	std::map<std::string, SSB::Model*>	m_List;

public:
	HRESULT Load(std::string name, std::string anim, SSB::Model** retModel);
	SSB::Model* Find(std::string name);
	bool	Release();

private:
	ModelMgr();
public:
	~ModelMgr();
};

#define I_Model ModelMgr::GetInstance()

