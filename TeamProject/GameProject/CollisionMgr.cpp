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
	for (auto iter : m_NpcList)
	{
		T_BOX* npcBox = iter.first;
		if (npcBox != box)
		{
			if (TCollision::ChkOBBToOBB(*box, *npcBox))
			{
				return true;
			}
		}
	}

	for (auto iter : m_MapCollisionList)
	{
		if (TCollision::ChkOBBToOBB(*box, iter))
		{
			return true;
		}
	}

	return false;
}

bool CollisionMgr::IsCollideTrigger(T_BOX* box)
{
	for (auto iter : m_MapTriggerList)
	{
		if (TCollision::ChkOBBToOBB(*box, iter.second))
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

void CollisionMgr::AddMapTriggerBox(std::wstring szName, T_BOX tBox)
{
	m_MapTriggerList.insert(std::make_pair(szName, tBox));
}

void CollisionMgr::DeleteTriggerBox(std::wstring szName)
{
	auto iter = m_MapTriggerList.find(szName);
	if (iter != m_MapTriggerList.end())
		m_MapTriggerList.erase(iter);
}

std::vector<T_BOX>& CollisionMgr::GetMapCollisionList()
{
	return m_MapCollisionList;
}


std::map<std::wstring, T_BOX>& CollisionMgr::GetMapTriggerList()
{
	return m_MapTriggerList;

bool CollisionMgr::IsPenetratable(std::vector<TVector3> planeVertice, T_PLANE plane, TVector3 vertex)
{
	planeVertice.push_back(planeVertice[0]);

	TVector3 edge1 = planeVertice[1] - planeVertice[0];
	TVector3 edge2 = vertex - planeVertice[0];

	TVector3 planeNormal(plane.fA, plane.fB, plane.fC);
	planeNormal.Normalize();
	TVector3 tmpVal = planeNormal * planeNormal.Dot(edge2);
	edge2 = edge2 - tmpVal;

	TVector3 standard = edge2.Cross(edge1);
	standard.Normalize();

	for (int i = 0; i < 4; ++i)
	{
		TVector3 edge1 = planeVertice[i + 1] - planeVertice[i];
		TVector3 edge2 = vertex - planeVertice[i];

		TVector3 tmpVal = planeNormal * planeNormal.Dot(edge2);
		edge2 = edge2 - tmpVal;

		TVector3 out = edge2.Cross(edge1);
		out.Normalize();

		if (out.Dot(standard) < 0)
		{
			return false;
		}
	}
	return true;
}

bool CollisionMgr::IsPenetrate(T_PLANE plane, TVector3 start, TVector3 end)
{
	float startVal = plane.fA * start.x + plane.fB * start.y + plane.fC * start.z + plane.fD;
	float endVal = plane.fA * end.x + plane.fB * end.y + plane.fC * end.z + plane.fD;
	return startVal * endVal < 0;
}

std::vector<T_BOX> CollisionMgr::GetCollideBoxList(T_BOX* source, bool ghost)
{
	std::vector<T_BOX> ret;

	for (auto dest : m_StaticObjectList)
	{
		if (TCollision::ChkOBBToOBB(*source, *dest.first))
		{
			ret.push_back(*dest.first);
		}
	}
	for (auto dest : m_MapCollisionList)
	{
		if (TCollision::ChkOBBToOBB(*source, dest))
		{
			ret.push_back(dest);
		}
	}

	if (!ghost)
	{
		for (auto dest : m_NpcList)
		{
			if (dest.first != source)
			{
				if (TCollision::ChkOBBToOBB(*source, *dest.first))
				{
					ret.push_back(*dest.first);
				}
			}
		}
	}

	return ret;
}

std::vector<CollisionData> CollisionMgr::GetCollideData(T_BOX source, T_BOX dest)
{
	std::vector<CollisionData> ret;

	std::vector<std::vector<TVector3>> planeVertice;
	planeVertice.resize(6);
	// Clockwise
	planeVertice[0] = { dest.vPos[0], dest.vPos[1], dest.vPos[2], dest.vPos[3] };
	planeVertice[1] = { dest.vPos[7], dest.vPos[6], dest.vPos[5], dest.vPos[4] };
	planeVertice[2] = { dest.vPos[3], dest.vPos[2], dest.vPos[6], dest.vPos[7] };
	planeVertice[3] = { dest.vPos[4], dest.vPos[5], dest.vPos[1], dest.vPos[0] };
	planeVertice[4] = { dest.vPos[1], dest.vPos[5], dest.vPos[6], dest.vPos[2] };
	planeVertice[5] = { dest.vPos[4], dest.vPos[0], dest.vPos[3], dest.vPos[7] };

	TVector3 vertice[8][3];
	{
		vertice[0][0] = source.vPos[1];
		vertice[0][1] = source.vPos[4];
		vertice[0][2] = source.vPos[3];

		vertice[1][0] = source.vPos[0];
		vertice[1][1] = source.vPos[5];
		vertice[1][2] = source.vPos[2];

		vertice[2][0] = source.vPos[1];
		vertice[2][1] = source.vPos[3];
		vertice[2][2] = source.vPos[6];

		vertice[3][0] = source.vPos[0];
		vertice[3][1] = source.vPos[2];
		vertice[3][2] = source.vPos[7];

		vertice[4][0] = source.vPos[0];
		vertice[4][1] = source.vPos[5];
		vertice[4][2] = source.vPos[7];

		vertice[5][0] = source.vPos[1];
		vertice[5][1] = source.vPos[4];
		vertice[5][2] = source.vPos[6];

		vertice[6][0] = source.vPos[5];
		vertice[6][1] = source.vPos[2];
		vertice[6][2] = source.vPos[7];

		vertice[7][0] = source.vPos[3];
		vertice[7][1] = source.vPos[4];
		vertice[7][2] = source.vPos[6];
	}
	for (int i = 0; i < 8; ++i)
	{
		TVector3 start = source.vPos[i];
		for (int k = 0; k < 3; ++k)
		{
			TVector3 end = vertice[i][k];

			for (int j = 0; j < 4; ++j)
			{
				std::vector<TVector3> plane = planeVertice[j];
				if ((IsPenetratable(plane, dest.plane[j], start) || IsPenetratable(plane, dest.plane[j], end)) && IsPenetrate(dest.plane[j], start, end))
				{
					bool exist = false;
					for (auto& already : ret)
					{
						if (fabs(already.CollisionNormal.x - dest.plane[j].fA) < 0.001f &&
							fabs(already.CollisionNormal.y - dest.plane[j].fB) < 0.001f &&
							fabs(already.CollisionNormal.z - dest.plane[j].fC) < 0.001f)
						{
							exist = true;

							T_PLANE plane = dest.plane[j];
							float val1 = plane.fA * end.x + plane.fB * end.y + plane.fC * end.z + plane.fD;
							float val2 = plane.fA * start.x + plane.fB * start.y + plane.fC * start.z + plane.fD;
							float val = min(val1, val2);

							already.CollisionDepth = max(already.CollisionDepth, abs(val));
						}
					}

					if (!exist)
					{
						T_PLANE plane = dest.plane[j];
						float val1 = plane.fA * end.x + plane.fB * end.y + plane.fC * end.z + plane.fD;
						float val2 = plane.fA * start.x + plane.fB * start.y + plane.fC * start.z + plane.fD;
						float val = min(val1, val2);
						ret.push_back(CollisionData{TVector3(dest.plane[j].fA, dest.plane[j].fB, dest.plane[j].fC), abs(val)});
					}
				}
			}
		}
	}

	return ret;
}

void CollisionMgr::NPCIsDead(T_BOX* box)
{
	m_NpcList.erase(box);
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