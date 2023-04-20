#include "CameraCinema.h"

void CameraCinema::MoveCameraBezierSpline(float time, float duration, XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 d0, XMFLOAT3 d1, XMFLOAT3 d2, XMFLOAT3 d3, XMFLOAT3& getPos, XMFLOAT3& getDir)
{
	float u = time / duration;
	float u1 = 1.0f - u;
	float u2 = u1 * u1;
	float u3 = u * u;
	float b1 = u1 * u1 * u1;
	float b2 = 3 * u1 * u1 * u;
	float b3 = 3 * u1 * u * u;
	float b4 = u * u * u;

	XMFLOAT3 pos(
		b1 * p0.x + b2 * p1.x + b3 * p2.x + b4 * p3.x,
		b1 * p0.y + b2 * p1.y + b3 * p2.y + b4 * p3.y,
		b1 * p0.z + b2 * p1.z + b3 * p3.z + b4 * p3.z
	);

	XMFLOAT3 dir(
		b1 * d0.x + b2 * d1.x + b3 * d2.x + b4 * d3.x,
		b1 * d0.y + b2 * d1.y + b3 * d2.y + b4 * d3.y,
		b1 * d0.z + b2 * d1.z + b3 * d2.z + b4 * d3.z
	);
	getPos = pos;
	getDir = dir;
}

bool CameraCinema::Frame()
{
	XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR rotation =
		XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(m_fCameraPitchAngle),
			XMConvertToRadians(m_fCameraYawAngle),
			XMConvertToRadians(m_fCameraRollAngle));

	XMVECTOR translation = m_vPos;;

	m_matWorld = XMMatrixTransformation({ 0,0,0,0 }, { 0,0,0,0 }, scale, { 0,0,0,0 }, rotation, translation);
	m_matView = m_matWorld.Invert();

	m_vRight = TVector3(m_matView._11, m_matView._21, m_matView._31);
	m_vRight.Normalize();

	m_vUp = TVector3(m_matView._12, m_matView._22, m_matView._32);
	m_vUp.Normalize();

	m_vTarget = TVector3(m_matView._13, m_matView._23, m_matView._33);
	m_vTarget.Normalize();

	m_vFrustum.CreateFrustum(&m_matView, &m_matProj);
    return true;
}

CameraCinema::CameraCinema() : Camera()
{
}

CameraCinema::~CameraCinema()
{
}
