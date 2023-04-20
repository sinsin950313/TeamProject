#pragma once
#include "Effect.h"

class EffectMgr : public Singleton<EffectMgr>
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	void	SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	void	SetCamera(Camera* pCamera);

	Camera* m_pCamera;

private:
	friend class Singleton<EffectMgr>;
	std::list<Effect*>	m_List;

public:
	void	CreateEffect(std::wstring path, TVector3 vPos);

	bool	Frame();
	bool	Render();
	bool	Release();
private:
	std::wstring ChangePath(std::wstring fullpath);
private:
	EffectMgr();
public:
	~EffectMgr();
};

#define I_Effect EffectMgr::GetInstance()

