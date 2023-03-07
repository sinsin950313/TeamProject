#include "CCameraDebug.h"
#include "Input.h"
#include "TMath.h"

//void CCameraDebug::CreateViewMatrix(TVector3 vEye, TVector3 vAt, TVector3 vUp)
//{
//	m_vPos = vEye;
//	m_vTarget = vAt;
//	m_vUp = vUp;
//	m_matView.ViewLookAt(vEye, vAt, vUp);
//}
//
//void CCameraDebug::CreateProjMatrix(float fNear, float fFar, float fFovY, float fAspectRatio)
//{
//	m_fNear = fNear;
//	m_fFar = fFar;
//	m_fFovY = fFovY;
//	m_fAspectRatio = fAspectRatio;
//	m_matProj = CYJMATH::PerspectiveFovLH(m_fNear, m_fFar, m_fFovY, m_fAspectRatio);
//}

bool CCameraDebug::Frame()
{
	if (I_Input.GetKey(VK_SPACE) == KEY_HOLD)
	{
		m_fSpeed += g_fSecondPerFrame * 100.0f;
	}
	else
	{
		m_fSpeed -= g_fSecondPerFrame * 100.0f;
	}
	m_fSpeed = max(10.0f, m_fSpeed);
	m_fSpeed = min(100.0f, m_fSpeed);

	if (I_Input.GetKey('W') == KEY_HOLD)
	{
		m_vPos += m_vLook * m_fSpeed * g_fSecondPerFrame;
	}
	if (I_Input.GetKey('S') == KEY_HOLD)
	{
		m_vPos -= m_vLook * m_fSpeed * g_fSecondPerFrame;
	}
	if (I_Input.GetKey('A') == KEY_HOLD)
	{
		m_vPos -= m_vRight * m_fSpeed * g_fSecondPerFrame;
	}
	if (I_Input.GetKey('D') == KEY_HOLD)
	{
		m_vPos += m_vRight * m_fSpeed * g_fSecondPerFrame;
	}
	if (I_Input.GetKey('Q') == KEY_HOLD)
	{
		m_vPos += m_vUp * m_fSpeed * g_fSecondPerFrame;
	}
	if (I_Input.GetKey('E') == KEY_HOLD)
	{
		m_vPos -= m_vUp * m_fSpeed * g_fSecondPerFrame;
	}
	if (I_Input.GetKey(VK_RBUTTON) == KEY_HOLD)
	{
		m_vRotation.y += I_Input.m_ptOffset.x * 0.002f;
		m_vRotation.x += I_Input.m_ptOffset.y * 0.002f;
	}

	TVector3 vPos = m_vPos;
	TMatrix matWorld;
	TMatrix matView;
	TMatrix matRotation;
	TQuaternion m_qRotation;
	D3DXQuaternionRotationYawPitchRoll(&m_qRotation, m_vRotation.y, m_vRotation.x, m_vRotation.z);
	D3DXMatrixAffineTransformation(&matWorld, 1.0f, NULL, &m_qRotation, &vPos); 
	D3DXMatrixInverse(&matView, NULL, &matWorld);
	m_matView = *((TMatrix*)&matView);
	

	UpdateVector();
	return true;
}