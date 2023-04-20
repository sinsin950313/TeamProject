#pragma once
#include "Emitter.h"

class Effect
{
public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	void	Reset();

public:
	std::vector<Emitter*>	m_pEmitterList;

	TVector3	m_vPos;
};

