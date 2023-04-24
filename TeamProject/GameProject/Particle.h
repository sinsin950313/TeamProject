#pragma once
#include "TMath.h"

// https://copynull.tistory.com/290
// https://copynull.tistory.com/297
// 
// Sprite << 애니메이션
// 이미시브? < 글로우 / 광선
// 
// 위치: 3D 공간에서 입자의 위치입니다.
// 속도: 입자가 움직이는 속도와 방향.
// 가속도 : 입자의 속도가 변하는 속도입니다.
// 질량 : 입자에 포함된 물질의 양.
// 수명 : 입자가 파괴되기 전에 존재하는 시간입니다.
// 크기 : 종종 반지름 또는 경계 상자로 표시되는 입자의 크기입니다.
// 색상 : 시간이 지남에 따라 다양한 효과를 내기 위해 변할 수 있는 입자의 색상입니다.
// 텍스처 : 파티클을 렌더링하는 데 사용되는 이미지 또는 패턴으로, 시간이 지남에 따라 변경될 수도 있습니다.
// 
// 원하는 입자 동작에 따라 포함될 수 있는 추가 구성 요소는 다음과 같습니다.
// 회전 : 자체 축을 중심으로 하는 입자의 회전입니다.
// 
// 회전 : 입자가 회전하는 속도입니다.
// 댐핑 : 입자의 속도가 시간이 지남에 따라 감소하는 속도입니다.
// 힘 : 중력이나 바람과 같이 입자의 움직임에 영향을 미치는 외부 힘.
// 충돌 감지 : 입자가 장면의 다른 객체와 충돌하는 시기를 감지하는 기능입니다.
// 

/*
function getPoint() {
	var u = Math.random();
	var v = Math.random();
	var theta = u * 2.0 * Math.PI;
	var phi = Math.acos(2.0 * v - 1.0);
	var r = Math.cbrt(Math.random());
	var sinTheta = Math.sin(theta);
	var cosTheta = Math.cos(theta);
	var sinPhi = Math.sin(phi);
	var cosPhi = Math.cos(phi);
	var x = r * sinPhi * cosTheta;
	var y = r * sinPhi * sinTheta;
	var z = r * cosPhi;
	return { x: x, y : y, z : z };
}

def random_three_vector() {
	"""
		Generates a random 3D unit vector(direction) with a uniform spherical distribution
		Algo from http ://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution
	: return :
		"""
		phi = np.random.uniform(0, np.pi * 2)
		costheta = np.random.uniform(-1, 1)

		theta = np.arccos(costheta)
		x = np.sin(theta) * np.cos(phi)
		y = np.sin(theta) * np.sin(phi)
		z = np.cos(theta)
		return (x, y, z)
}
*/

// distortion 쉐이더
// 모양? shape? 타입?
// 
//

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
	void	ProcessColor();
	void	ProcessSRT();
	void	ProcessBillboard();

	// 파티클 변수
	//z_roll회전변수
	//파티클위치에 사운드위치를 고려할것인가
	//그냥Loop(무한)인가, 횟수지정반복인가
	//이펙트 스프라이트의경우 특정 인덱스를 삭제할것인가
	//스폰후 몇초뒤부터 이펙트가시작될것인가 delaytime
	//이펙트의 dx11블렌드옵션, 어떻게처리할것인가
	// 
	
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
	TMatrix			m_matWorld;
	TMatrix			m_matView;

	TMatrix			m_matRotation;
	TMatrix			m_matBillboard;
};

