#include "Player.h"
#include "Input.h"
#include "CollisionMgr.h"

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
	m_pTrail->Create(m_pd3dDevice, m_pImmediateContext, L"../../data/shader/TrailEffect.txt", L"../../data/swoosh.png");
	m_pTrail->Init();

	return true;
}

bool    Player::Frame()
{
	Character::Frame();
	m_pTrail->Frame();

	//
	// 행렬 배열? 큐? 리스트?를 플레이어 전용 쉐이더 파일을 만들어서 넘기고
	// 알파값 및 색상을 가까움 ~ 멂으로 조절 
	// 픽셀 쉐이더에서 픽셀 그리기
	//

	return true;
}