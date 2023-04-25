#include "CameraTPS.h"
#include "Input.h"

bool CameraTPS::Frame()
{
	m_fCameraYawAngle += I_Input.m_ptOffset.x * g_fSecondPerFrame;
	m_fCameraPitchAngle += I_Input.m_ptOffset.y * g_fSecondPerFrame;
	// Check that the camera doesn't go over the top or under the player
	if (m_fCameraPitchAngle > 0.85f)
		m_fCameraPitchAngle = 0.85f;
	//if (m_fCameraPitchAngle < -0.85f)
	//	m_fCameraPitchAngle = -0.85f;
	if (m_fCameraPitchAngle < 0.10f)
		m_fCameraPitchAngle = 0.10f;

	// Third Person Camera
	// Set the cameras target to be looking at the character.
	//m_vTarget = { 0, 0, 0 };//charPosition;
	if(m_vFollowPos)
		m_vTarget = *m_vFollowPos;

	// This line is because this lessons model was set to stand on the point (0,0,0) (my bad), and we
	// don't want to just be looking at the models feet, so we move the camera's target vector up 5 units
	//m_vTarget = XMVectorSetY(m_vTarget, XMVectorGetY(m_vTarget) + 5.0f);
	m_vTarget.y; // += 5.0f

	//m_fCameraYawAngle;
	//m_fCameraPitchAngle;
	//m_fCameraRollAngle;

	// Unlike before, when we rotated the cameras target vector around the cameras position,
	// we are now rotating the cameras position around it's target (which is the character)
	// Rotate camera around target
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(-m_fCameraPitchAngle, m_fCameraYawAngle, 0);
	XMVECTOR DefaultForward = { 0, 0, 1, 0 };
	XMVECTOR camPosition = XMVector3TransformNormal(DefaultForward, camRotationMatrix);
	camPosition = XMVector3Normalize(camPosition);

	// Set our cameras position to rotate around the character. We need to add 5 to the characters
	// position's y axis because i'm stupid and modeled the character in the 3d modeling program
	// to be "standing" on (0,0,0), instead of centered around it ;) Well target her head here though
	//m_CharCamDist = 10.0f;
	camPosition = (camPosition * m_CharCamDist) + m_vTarget;

	// We need to set our cameras forward and right vectors to lay
	// in the worlds xz plane, since they are the vectors we will
	// be using to determine the direction our character is running
	XMVECTOR forward = XMVector3Normalize(m_vTarget - camPosition);	// Get forward vector based on target
	forward = XMVectorSetY(forward, 0.0f);	// set forwards y component to 0 so it lays only on
	// the xz plane
	forward = XMVector3Normalize(forward);
	XMStoreFloat3(&m_vLook, forward);
	// To get our camera's Right vector, we set it's x component to the negative z component from the
	// camera's forward vector, and the z component to the camera forwards x component
	XMVECTOR right = XMVectorSet(-XMVectorGetZ(forward), 0.0f, XMVectorGetX(forward), 0.0f);
	XMStoreFloat3(&m_vRight, right);

	// Our camera does not "roll", so we can safely assume that the cameras right vector is always
	// in the xz plane, so to get the up vector, we just get the normalized vector from the camera
	// position to the cameras target, and cross it with the camera's Right vector
	XMVECTOR up = XMVector3Normalize(XMVector3Cross(XMVector3Normalize(camPosition - m_vTarget), m_vRight));
	XMStoreFloat3(&m_vUp, up);

	XMStoreFloat3(&m_vPos, camPosition);
	UpdateCameraShake();
	m_matView = XMMatrixLookAtLH(m_vPos, m_vTarget, m_vUp);
	m_vRight = -m_vRight;

	m_vFrustum.CreateFrustum(&m_matView, &m_matProj);

	{
		TQuaternion q;
		D3DXQuaternionRotationYawPitchRoll(&q, m_vRotation.y, m_vRotation.x, m_vRotation.z);
		D3DXMatrixAffineTransformation(&m_matWorld, 1, nullptr, &q, &m_vPos);
	}

	return true;
}

