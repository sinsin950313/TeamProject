#include "Player.h"
#include "Input.h"
#include "CollisionMgr.h"

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

	return true;
}

bool    Player::Frame()
{
	Character::Frame();

	return true;
}