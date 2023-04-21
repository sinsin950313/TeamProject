#include "CollisionMgr.h"
#include "Player.h"

CollisionMgr::CollisionMgr()
{

}
CollisionMgr::~CollisionMgr()
{

}

	//
	// 각 객체가 CollisionMgr에 본인의 박스를 원하는 리스트에 충돌검사를 해달라는 요청 후 피드백으로 결과 받기
	//

bool	CollisionMgr::ChkPlayerAttackToNpcList(T_BOX* box)
{
	for (auto iter : m_NpcList)
	{
		T_BOX* npcBox = iter.first;
		if (TCollision::ChkOBBToOBB(*box, *npcBox))
		{
			// NPC의 피격 이벤트 활성화?
			// 아니면 리턴 받은 곳에서 true false로 체크

			return true;
		}
	}
	return false;
}

bool	CollisionMgr::ChkCharacterToStaticObjList(T_BOX* box)
{
	for (auto iter : m_StaticObjectList)
	{
		T_BOX* staticBox = iter.first;
		if (TCollision::ChkOBBToOBB(*box, *staticBox))
		{
			return true;
		}
	}
	return false;
}

std::vector<Character*> CollisionMgr::GetHitCharacterList(T_BOX* attackBox)
{
	std::vector<Character*> ret;
	for (auto iter : m_NpcList)
	{
		T_BOX* npcBox = iter.first;
		if (TCollision::ChkOBBToOBB(*attackBox, *npcBox))
		{
			ret.push_back(iter.second);
		}
	}
	return ret;
}

bool CollisionMgr::IsCollide(T_BOX* box)
{
	// NPC간의 충돌 시 문제가 있음
	//for (auto iter : m_NpcList)
	//{
	//	T_BOX* npcBox = iter.first;
	//	if (npcBox != box)
	//	{
	//		if (TCollision::ChkOBBToOBB(*box, *npcBox))
	//		{
	//			return true;
	//		}
	//	}
	//}

	for (auto iter : m_MapCollisionList)
	{
		if (TCollision::ChkOBBToOBB(*box, iter))
		{
			return true;
		}
	}

	return false;
}

void CollisionMgr::AddMapCollisionBox(T_BOX tBox)
{
	m_MapCollisionList.push_back(tBox);
}

std::vector<T_BOX>& CollisionMgr::GetMapCollisionList()
{
	return m_MapCollisionList;
}

std::vector<T_BOX*> CollisionMgr::GetCollideBoxList(T_BOX* source)
{
	std::vector<T_BOX*> ret;

	for (auto dest : m_StaticObjectList)
	{
		if (TCollision::ChkOBBToOBB(*source, *dest.first))
		{
			ret.push_back(dest.first);
		}
	}
	for (auto dest : m_MapCollisionList)
	{
		if (TCollision::ChkOBBToOBB(*source, dest))
		{
			ret.push_back(&dest);
		}
	}
	for (auto dest : m_NpcList)
	{
		if (TCollision::ChkOBBToOBB(*source, *dest.first))
		{
			ret.push_back(dest.first);
		}
	}

	return ret;
}

std::vector<TVector3> CollisionMgr::GetCollideNormal(T_BOX* source, TVector3 delta, T_BOX* dest)
{
	std::vector<TVector3> ret;

	for (int i = 0; i < 8; ++i)
	{
		TVector3 start = source->vPos[i];
		TVector3 end = source->vPos[i] + delta;

		for (int j = 0; j < 6; ++j)
		{
			T_PLANE plane = dest->plane[j];
			if (IsPenetratable(plane, start) && IsPenetratable(plane, end) && IsPenetrate(plane, start, end))
			{
				TVector3 normal(dest->plane[j].fA, dest->plane[j].fB, dest->plane[j].fC);
				ret.push_back(normal);
			}
		}
	}

	return ret;
}

void	CollisionMgr::AddStaticObjectBox(T_BOX* box, Character* pChar)
{
	if (m_StaticObjectList.find(box) == m_StaticObjectList.end())
	{
		m_StaticObjectList.insert(std::make_pair(box, pChar));
	}
}

void	CollisionMgr::AddNpcBox(T_BOX* box, Character* pChar)
{
	if (m_NpcList.find(box) == m_NpcList.end())
	{
		m_NpcList.insert(std::make_pair(box, pChar));
	}
}

void	CollisionMgr::DeleteBox(T_BOX* box)
{
	//test.erase(box);
	//int a = 4;
}