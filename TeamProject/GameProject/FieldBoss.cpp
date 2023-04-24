#include "FieldBoss.h"
#include "MeshMap.h"

namespace SSB
{
	float FieldBoss::GetBattleRange()
	{
		return m_BattleRange;
	}
	float FieldBoss::GetSpotRange()
	{
		return m_SpotRange;
	}
	void FieldBoss::Attack()
	{
		_arrows.insert(new Arrow(m_pMap, g_fGameTimer));
	}
	void FieldBoss::SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj)
	{
		Character::SetMatrix(matWorld, matView, matProj);
		for (auto arrow : _arrows)
		{
			arrow->SetMatrix(matWorld, matView, matProj);
		}
	}
	bool FieldBoss::Frame()
	{
		std::set<Arrow*> eraseSet;
		for (auto arrow : _arrows)
		{
			if (arrow->IsDead())
			{
				eraseSet.insert(arrow);
			}
			else
			{
				arrow->Frame();
			}
		}

		for (auto erase : eraseSet)
		{
			erase->Release();
			_arrows.erase(erase);
			delete erase;
		}
	}
	bool FieldBoss::Release()
	{
		for (auto arrow : _arrows)
		{
			arrow->Release();
			delete arrow;
		}
		_arrows.clear();
	}
	FieldBoss::Arrow::Arrow(MeshMap* map, float timeStamp) : _startTimeStamp(timeStamp)
	{
		SetMap(map);
	}
	bool FieldBoss::Arrow::IsDead()
	{
		if (m_pMap->GetHeight() < currentHeight)
			return true;
		if (_kLiveTime < g_fGameTimer - _startTimeStamp)
			return true;
		return false;
	}
	bool FieldBoss::Arrow::Frame()
	{
		Move and Collision Check;
	}
}
