#include "CollisionMgr.h"
#include "Player.h"

CollisionMgr::CollisionMgr()
{

}
CollisionMgr::~CollisionMgr()
{

}

bool	CollisionMgr::Frame()
{
	Player* player = &Player::GetInstance();

	// static mesh -> player check
	// 충돌시 플레이어의 방향 반대편으로 이동속도만큼 뒤로 
	for (auto iter : m_StaticObjectList)
	{
		T_BOX box = *iter.first;
		if (TCollision::ChkOBBToOBB(box, player->m_ColliderBox))
		{
			player->m_vPos = player->m_vPos + (-player->m_vDirection * player->m_fSpeed * g_fSecondPerFrame);
			player->UpdateMatrix();
			//player->UpdateBox();
			//player->UpdateBuffer();
		}
	}

	// player attack -> npc check
	// 충돌시 npc에게 데미지 이후 데미지 받은 npc는 무적 판정이나 같은 공격에 연속 데미지 방지
	// 평타에 광역 판정을 넣으면 이후 충돌 npc도 데미지 단일 타겟이면 이후 공격 판정 삭제
	// 아마 광역 판정으로 할듯?

	// npc attack -> player check
	// npc 어택(근거리) 플레이어에게 데미지 후 무적판정
	// 플레이어가 데미지를 받았으면 break해도 될듯

	// 

	return true;
}
void	CollisionMgr::AddBox(T_BOX* box, Character* pChar)
{
	if (test.find(box) == test.end())
	{
		test.insert(std::make_pair(box, pChar));

	}
	int a = 4;
}

void	CollisionMgr::AddStaticObjectBox(T_BOX* box, Character* pChar)
{
	if (m_StaticObjectList.find(box) == m_StaticObjectList.end())
	{
		m_StaticObjectList.insert(std::make_pair(box, pChar));

	}
}

void	CollisionMgr::DeleteBox(T_BOX* box)
{
	test.erase(box);
	int a = 4;
}