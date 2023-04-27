#pragma once
#include "Emitter.h"

class Effect
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;

public:
	bool	Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	bool	Frame();
	bool	Render();
	bool	Release();

	void	SetCamera(Camera* pCamera);
	void	Reset();

public:
	std::list<Emitter*>	m_pEmitterList;

	TMatrix		m_matTopWorld;

	TVector3	m_vPos;
	TVector3* m_vFollowPos;
	bool			m_isDone;
};

