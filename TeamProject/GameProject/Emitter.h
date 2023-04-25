#pragma once
#include "Camera.h"
#include "Sprite.h"
#include "Particle.h"
#include "EmitterData.h"

struct VS_EFFECT_BUFFER
{
	TMatrix matInst[500];
	TVector4 vColor[500];
};

class Emitter
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

	TVector3 GetRandPos(TVector3 diff);
	double	GetRandRealNum(double base, double diff);
	TColor	GetRandColor(TColor c1, TColor c2);

	void	SpawnParticle();
	void	ParticleFrame();
	void	CalcInheritMatrix(Emitter* pChild);

	void	Reset();

public:
	Sprite* m_pSprite; // 랜더링 스프라이트
	Camera* m_pCamera;
	std::vector<Emitter*>	m_pChild;
	TMatrix		m_matParentTrans;
	TMatrix		m_matParentRotation;
	TMatrix		m_matParentScale;

public:
		// 파티클 매니저 변수
	BasicData			m_BasicData;
	TransformData	m_TransData[3];
	BasicRenderData m_BasicRenderData;
	RenderSetData	m_RenderSetData;

	ID3D11Buffer* m_pInstancingBuffer;
	VS_EFFECT_BUFFER m_cbInstancingData;
	std::list<Particle*>	m_pParticleList;

	int					m_iCurSpawn;
	float				m_fSpawnTime;
	float				m_fSpawnTimer;

	float				m_fCurInitTime;
	float				m_fInitTime;

	bool				m_isDone;

	///////////////////////////////////
	bool				m_isLifeColor;
	bool				m_isLifeAlpha;
};

