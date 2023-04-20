#include "Particle.h"
#include "Timer.h"

Particle::Particle()
{

}
Particle::~Particle()
{

}

void	Particle::Create(ParticleData data)
{
	for (int i = 0; i < 3; i++)
	{
		m_iTransformType[i] = data.iTransformType[i];
		m_ParticleSRT[i] = data.ParticleSRT[i];
	}

	m_StartColor = data.StartColor;
	m_EndColor = data.EndColor;
	m_isLifeColor = data.isLifeColor;
	m_isLifeAlpha = data.isLifeAlpha;

	m_fCurLife = 0.0f;
	m_isDestroyLifeTime = data.isDestroyLifeTime;
	m_isDone = false;
	m_fLifeTime = data.fLifeTime;

	m_iBillBoardType = data.iBillBoardType;
}

bool	Particle::Frame(TMatrix* matView)
{
	if (matView != nullptr)
	{
		m_matView = (*matView);
	}
	// BillBoard 리턴 함수 (int type)
	// 
	//TVector3 vVelocity = (m_vAccel * g_fGameTimer) + m_vVelocity;
	//TVector3 Position = (1 / 2 * m_vAccel * powf(g_fGameTimer, 2)) + (m_vVelocity * g_fGameTimer) + m_vPos;

	ProcessSRT();
	ProcessBillboard();

	m_fCurLife += g_fSecondPerFrame;
	if (m_fCurLife > m_fLifeTime)
	{
		if (m_isDestroyLifeTime)
		{
			m_isDone = true;
		}
		
	}

	TQuaternion q = TQuaternion::CreateFromRotationMatrix(m_matRotation);
	D3DXMatrixAffineTransformation(&m_matWorld, &m_vScale, NULL, &q, &m_vPos);
	return true;
}

void	Particle::ProcessSRT()
{
	TVector3 vSRT[3];
	for (int i = 0; i < 3; i++)
	{
		if (m_iTransformType[i] == 0) // FIX
		{
			vSRT[i] = m_ParticleSRT[i].vFix;
		}
		else if (m_iTransformType[i] == 1) // PVA
		{
			m_ParticleSRT[i].vCurVelocity += m_ParticleSRT[i].vVelocity + m_ParticleSRT[i].vAccel * powf(g_fSecondPerFrame, 2);
			m_ParticleSRT[i].vPVA += m_ParticleSRT[i].vCurVelocity * g_fSecondPerFrame;
			vSRT[i] = m_ParticleSRT[i].vPVA;
		}
		else if (m_iTransformType[i] == 2) // Easing
		{
			D3DXVec3Lerp(&vSRT[i], &m_ParticleSRT[i].vStart, &m_ParticleSRT[i].vEnd, m_fCurLife / m_fLifeTime);
		}
	}
	m_vScale = vSRT[2];
	m_vRotation = vSRT[1];
	m_vPos = vSRT[0];
}

void	Particle::ProcessBillboard()
{
	switch (m_iBillBoardType)
	{
	case 0: // Billboard
		m_matRotation = m_matView.Invert();
		m_matRotation._41 = m_matRotation._42 = m_matRotation._43 = 0.0f;
		break;
	case 1: // Rotate Billboard
		D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_vRotation.y, m_vRotation.x, m_vRotation.z);
		m_matBillboard = m_matView.Invert();
		m_matBillboard._41 = m_matBillboard._42 = m_matBillboard._43 = 0.0f;
		D3DXMatrixMultiply(&m_matRotation, &m_matRotation, &m_matBillboard);
		break;
	case 2: // Y Axis Billboard		
	{
		m_matBillboard = m_matView.Invert();
		m_matBillboard._41 = m_matBillboard._42 = m_matBillboard._43 = 0.0f;
		TVector3 s, t;
		TQuaternion q;
		m_matBillboard.Decompose(s, q, t);
		q.x = q.z = 0.0f;
		D3DXMatrixRotationQuaternion(&m_matRotation, &q);
	}
		break;
	case 3: // Fix
	default:
		D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_vRotation.y, m_vRotation.x, m_vRotation.z);
		break;
	}
}