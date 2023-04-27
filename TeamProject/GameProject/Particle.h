#pragma once
#include "TMath.h"
#include "Emitter.h"
struct ParticleSRT
{
	TVector3		vFix;

	TVector3		vPVA;
	TVector3		vCurVelocity;
	TVector3		vVelocity;
	TVector3		vAccel;

	TVector3		vStart;
	TVector3		vEnd;
};
struct ParticleColor
{
	UINT		iColorType;

	TColor	vFix;

	TColor	vRandom;

	TColor	vStart;
	TColor	vEnd;
};
struct ParticleData
{
	UINT				iTransformType[3];
	ParticleSRT	particleSRT[3];

	ParticleColor	ColorData;

	bool				isDestroyLifeTime;
	float				fLifeTime;

	UINT				iBillBoardType;
};

class Particle
{
public:
	Particle();
	~Particle();

	void	Create(ParticleData data);

	bool	Frame(TMatrix* matView);
	bool	Render();
	bool	Release();
	void	ProcessColor();
	void	ProcessSRT();
	void	ProcessBillboard();

	void	CalcInheritMatrix(Emitter* pChild);

	// 파티클 변수
	//z_roll회전변수
	//파티클위치에 사운드위치를 고려할것인가
	//그냥Loop(무한)인가, 횟수지정반복인가
	//이펙트 스프라이트의경우 특정 인덱스를 삭제할것인가
	//스폰후 몇초뒤부터 이펙트가시작될것인가 delaytime
	//이펙트의 dx11블렌드옵션, 어떻게처리할것인가
	// 
	std::list<Emitter*>		m_pChildEmitterList;

	TVector3		m_vPos;
	TVector3		m_vScale;
	TVector3		m_vRotation;

	TColor			m_vCurColor;

	UINT				m_iTransformType[3];
	ParticleSRT	m_ParticleSRT[3];
	ParticleColor	m_ParticleColor;

	bool				m_isDestroyLifeTime;
	bool				m_isDone;
	float				m_fCurLife;
	float				m_fLifeTime;

	UINT				m_iBillBoardType;

public:
	TMatrix			m_matLocal;
	TMatrix			m_matParent;

	TMatrix			m_matWorld;
	TMatrix			m_matView;

	TMatrix			m_matScale;
	TMatrix			m_matRotation;
	TMatrix			m_matTrans;
	TMatrix			m_matBillboard;

	TMatrix			m_matParentScale;
	TMatrix			m_matParentRotation;
	TMatrix			m_matParentTrans;

	TMatrix*		m_matTopWorld;
};

