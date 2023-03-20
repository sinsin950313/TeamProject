#include "Player.h"
#include "Input.h"

Player::Player()
{

}

Player::~Player()
{
	Release();
}

bool    Player::Init()
{
	Character::Init();
	m_AttackBox.CreateOBBBox(1, 1, 1);
	return true;
}

bool    Player::Frame()
{
	Character::Frame();
	return true;
}

void    Player::Attack()
{

}