#include "Player.h"
#include "Input.h"

bool    Player::Init()
{
	Character::Init();
	m_fSpeed = 15.0f;
	m_AttackBox.CreateOBBBox(1, 1, 1);
	return true;
}

bool    Player::Frame()
{
	float speed = 10.0f * g_fSecondPerFrame;
	bool moveChar = false;
	XMVECTOR desiredCharDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	if (I_Input.GetKey('A') == KEY_HOLD)
	{
		desiredCharDir += (m_pMainCamera->m_vRight);
		moveChar = true;
	}
	if (I_Input.GetKey('D') == KEY_HOLD)
	{
		desiredCharDir += -(m_pMainCamera->m_vRight);
		moveChar = true;
	}
	if (I_Input.GetKey('W') == KEY_HOLD)
	{
		desiredCharDir += (m_pMainCamera->m_vLook);
		moveChar = true;
	}
	if (I_Input.GetKey('S') == KEY_HOLD)
	{
		desiredCharDir += -(m_pMainCamera->m_vLook);
		moveChar = true;
	}

	if (I_Input.GetKey(VK_LBUTTON) == KEY_PUSH && !m_isAttack)
	{
		m_isAttack = true;
		// 공격 애니메이션으로 변경
		// 해당 애니메이션 프레임이 끝날때까지 공격 상태 유지
		// 공격 중 특정 구간에서 툴에서 지정한 포인트 판정하는 기능

	}
	static float temp = 0.0f;
	temp += g_fSecondPerFrame;
	TVector3 cen;
	TMatrix rot;
	D3DXMatrixRotationYawPitchRoll(&rot, temp, 0, 0);
	cen = m_vPos - m_matWorld.Right() * 2.0f;
	m_AttackBox.CreateOBBBox(0, 0, 0, cen);
	if (moveChar == true)
	{
		XMMATRIX world = XMLoadFloat4x4(&m_matWorld);
		MoveChar(desiredCharDir, world);
		//m_matWorld = world;
	}
	if (m_isAttack)
	{
		Attack();
	}

	return Character::Frame();
}

void    Player::Attack()
{

}

void Player::MoveChar(XMVECTOR& destinationDirection, XMMATRIX& worldMatrix)
{
	float frameTime = g_fSecondPerFrame;
	static XMVECTOR oldCharDirection = XMVectorZero();

	destinationDirection = XMVector3Normalize(destinationDirection);

	if (XMVectorGetX(XMVector3Dot(destinationDirection, oldCharDirection)) == -1)
		oldCharDirection += XMVectorSet(0.12f, 0.0f, -0.12f, 0.0f);

	XMVECTOR charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	charPosition = XMVector3TransformCoord(charPosition, worldMatrix);

	float destDirLength = 15.0f * frameTime;

	XMVECTOR currCharDirection = (oldCharDirection)+(destinationDirection * destDirLength);	// Get the characters direction (based off time, old position, and desired
	
	currCharDirection = XMVector3Normalize(currCharDirection);

	XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	float charDirAngle = XMVectorGetX(XMVector3AngleBetweenNormals(XMVector3Normalize(currCharDirection), XMVector3Normalize(DefaultForward)));
	if (XMVectorGetY(XMVector3Cross(currCharDirection, DefaultForward)) > 0.0f)
		charDirAngle = -charDirAngle;

	float speed = m_fSpeed * frameTime;
	charPosition = charPosition + (destinationDirection * speed);

	//// Update characters world matrix
	//XMMATRIX rotationMatrix;
	////XMMATRIX Scale = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	//XMMATRIX Scale = XMMatrixIdentity();
	//XMMATRIX Translation = XMMatrixTranslation(XMVectorGetX(charPosition), 0.0f, XMVectorGetZ(charPosition));
	//rotationMatrix = XMMatrixRotationY(charDirAngle - 3.14159265f);		// Subtract PI from angle so the character doesn't run backwards

	m_vScale = TVector3(1, 1, 1);
	m_vRotation = TVector3(0, charDirAngle - XM_PI, 0);
	m_vPos = TVector3(XMVectorGetX(charPosition), 0, XMVectorGetZ(charPosition));

	// Set the characters old direction
	oldCharDirection = currCharDirection;
	m_vDirection = TVector3(XMVectorGetX(currCharDirection), XMVectorGetY(currCharDirection), XMVectorGetZ(currCharDirection));
}