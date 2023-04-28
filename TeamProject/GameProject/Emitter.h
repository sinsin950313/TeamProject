#pragma once
#include "Camera.h"
#include "Sprite.h"
#include "EmitterData.h"

class Particle;
#define INSTNUM 200
struct VS_EFFECT_BUFFER
{
	TMatrix matInst[INSTNUM];
	TVector4 vColor[INSTNUM];
	TVector4 vCalc[INSTNUM][2];
	TVector4 vUV[INSTNUM][2];
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

	Emitter* CopyEmitter(Emitter* pEmitter);
	void	SpawnParticle();
	void	ParticleFrame();

	void	SpriteReload();

	void	Reset();

public:
	Sprite* m_pSprite; // 랜더링 스프라이트
	Camera* m_pCamera;
	std::vector<Emitter*>	m_pChild;
	TMatrix		m_matParentTrans;
	TMatrix		m_matParentRotation;
	TMatrix		m_matParentScale;

	TMatrix*	m_matTopWorld;

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

	float				m_fLifeTime;
	float				m_fLifeTimer;

	bool				m_isDone;

	///////////////////////////////////
	bool				m_isLifeColor;
	bool				m_isLifeAlpha;
};

