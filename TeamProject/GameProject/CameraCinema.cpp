#include "CameraCinema.h"

void CameraCinema::MoveCameraBezierSpline(float time, float duration, std::vector<CameraMove>& camMoveList, XMFLOAT3& getPos, XMFLOAT3& getDir)
{
	float t = time / duration;
	int n = static_cast<int>(camMoveList.size()) - 1;
	float u = 1.0f - t;
	float un = pow(u, n);
	float tn = pow(t, n);
	XMFLOAT3 pos(0, 0, 0);
	XMFLOAT3 dir(0, 0, 0);
	for (int i = 0; i <= n; ++i)
	{
		float binomial = BinomialCoefficient(n, i);
		float basis = binomial * pow(u, n - i) * pow(t, i);
		pos.x += basis * camMoveList[i].camPos.x;
		pos.y += basis * camMoveList[i].camPos.y;
		pos.z += basis * camMoveList[i].camPos.z;

		dir.x += basis * camMoveList[i].fYaw;
		dir.y += basis * camMoveList[i].fPitch;
		dir.z += basis * camMoveList[i].fRoll;
	}

	getPos = pos;
	getDir = dir;
}

float CameraCinema::BinomialCoefficient(int n, int k)
{
	float result = 1;
	for (int i = 1; i <= k; ++i)
	{
		result *= (n - k + i) / static_cast<float>(i);
	}
	return result;
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
