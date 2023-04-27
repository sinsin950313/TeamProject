#include "Particle.h"
#include "Timer.h"
#include "Emitter.h"

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
		m_ParticleSRT[i] = data.particleSRT[i];
	}

	m_ParticleColor = data.ColorData;
	m_vCurColor = TColor(1, 1, 1, 1);

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

	ProcessColor();
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

	TMatrix matLocal;
	D3DXMatrixMultiply(&matLocal, &matLocal, &m_matScale);
	if (!(m_iBillBoardType == 0 || m_iBillBoardType == 2))
		D3DXMatrixMultiply(&matLocal, &matLocal, &m_matRotation);
	D3DXMatrixMultiply(&matLocal, &matLocal, &m_matTrans);
	m_matLocal = matLocal;

	TMatrix matParent;
	D3DXMatrixMultiply(&matParent, &matParent, &m_matParentScale);
	D3DXMatrixMultiply(&matParent, &matParent, &m_matParentRotation);
	D3DXMatrixMultiply(&matParent, &matParent, &m_matParentTrans);
	m_matParent = matParent;

	m_matWorld = m_matBillboard * matLocal * matParent * (*m_matTopWorld);

	auto iter = m_pChildEmitterList.begin();
	while (iter != m_pChildEmitterList.end())
	{
		CalcInheritMatrix(*iter);
		(*iter)->Frame();
		iter++;
	}
	return true;
}

bool	Particle::Render()
{
	auto iter = m_pChildEmitterList.begin();
	while (iter != m_pChildEmitterList.end())
	{
		(*iter)->Render();
		iter++;
	}
	return true;
}

bool	Particle::Release()
{
	auto iter = m_pChildEmitterList.begin();
	while (iter != m_pChildEmitterList.end())
	{
		(*iter)->Release();
		delete (*iter);
		iter++;
	}
	m_pChildEmitterList.clear();
	return true;
}

void	Particle::CalcInheritMatrix(Emitter* pChild)
{
	TMatrix matParentWorld;

	D3DXMatrixMultiply(&matParentWorld, &m_matLocal, &m_matParent);

	TVector3 scale, pos;
	TQuaternion rot;
	matParentWorld.Decompose(scale, rot, pos);
	TMatrix s, r, t;
	D3DXMatrixScaling(&s, scale.x, scale.y, scale.z);
	D3DXMatrixRotationQuaternion(&r, &rot);
	D3DXMatrixTranslation(&t, pos.x, pos.y, pos.z);

	if (pChild->m_BasicData.iInheritScaleType > 0)
		pChild->m_matParentScale = s;
	if (pChild->m_BasicData.iInheritRotType > 0)
		pChild->m_matParentRotation = r;
	if (pChild->m_BasicData.iInheritPosType > 0)
		pChild->m_matParentTrans = t;
}


void	Particle::ProcessColor()
{
	m_vCurColor;
	switch (m_ParticleColor.iColorType)
	{
	case 0: // FIX
		m_vCurColor = m_ParticleColor.vFix;
		break;
	case 1: // RANDOM
		m_vCurColor = m_ParticleColor.vRandom;
		break;
	case 2: // EASING
		float delta = m_fCurLife / m_fLifeTime;
		TVector4 color;
		TVector4 c1 = m_ParticleColor.vStart.ToVector4();
		TVector4 c2 = m_ParticleColor.vEnd.ToVector4();
		D3DXVec4Lerp(&color, &c1, &c2, delta);
		m_vCurColor = color;
		break;
	}
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
			//m_ParticleSRT[i].vCurVelocity = m_ParticleSRT[i].vVelocity;
			m_ParticleSRT[i].vCurVelocity += m_ParticleSRT[i].vAccel;
			m_ParticleSRT[i].vPVA += m_ParticleSRT[i].vCurVelocity * g_fSecondPerFrame;
			vSRT[i] = m_ParticleSRT[i].vPVA;
		}
		else if (m_iTransformType[i] == 2) // Easing
		{
			D3DXVec3Lerp(&vSRT[i], &m_ParticleSRT[i].vStart, &m_ParticleSRT[i].vEnd, m_fCurLife / m_fLifeTime);
		}
	}
	m_vScale = vSRT[2];
	D3DXMatrixScaling(&m_matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	m_vRotation = vSRT[1];
	D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_vRotation.y, m_vRotation.x, m_vRotation.z);
	m_vPos = vSRT[0];
	D3DXMatrixTranslation(&m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z);
}

void	Particle::ProcessBillboard()
{
	switch (m_iBillBoardType)
	{
	case 0: // Billboard
		m_matBillboard = m_matView.Invert();
		m_matBillboard._41 = m_matBillboard._42 = m_matBillboard._43 = 0.0f;
		break;
	case 1: // Rotate Billboard
	{
		TMatrix matRot;
		D3DXMatrixRotationYawPitchRoll(&matRot, m_vRotation.y, m_vRotation.x, m_vRotation.z);
		m_matBillboard = m_matView.Invert();
		m_matBillboard._41 = m_matBillboard._42 = m_matBillboard._43 = 0.0f;
		D3DXMatrixMultiply(&m_matBillboard, &matRot, &m_matBillboard);
	}
	break;
	case 2: // Y Axis Billboard		
	{
		m_matBillboard = m_matView.Invert();
		m_matBillboard._41 = m_matBillboard._42 = m_matBillboard._43 = 0.0f;
		TVector3 s, t;
		TQuaternion q;
		m_matBillboard.Decompose(s, q, t);
		q.x = q.z = 0.0f;
		D3DXMatrixRotationQuaternion(&m_matBillboard, &q);
	}
	break;
	case 3: // Fix
	default:
		D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_vRotation.y, m_vRotation.x, m_vRotation.z);
		break;
	}
}