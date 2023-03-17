#include "Player.h"
#include "Input.h"

bool    Player::Init()
{
	Character::Init();
	return true;
}

bool    Player::Frame()
{
	return Character::Frame();
}

void    Player::Attack()
{

}