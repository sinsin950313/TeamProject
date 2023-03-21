#include "Player.h"
#include "Input.h"

Player::Player()
{

}

Player::~Player()
{
	if (m_pTrail)
	{
		m_pTrail->Release();
		delete m_pTrail;
		m_pTrail = nullptr;
	}
	Release();
}

bool    Player::Init()
{
	Character::Init();
	m_AttackBox.CreateOBBBox(1, 1, 1);

	m_pTrail = new TrailEffect();
	m_pTrail->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/TrailEffect.txt", L"");
	m_pTrail->Init();
	return true;
}

bool    Player::Frame()
{
	m_pTrail->Frame();
	Character::Frame();
	return true;
}

void    Player::Attack()
{

}