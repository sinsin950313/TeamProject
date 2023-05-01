#include "EnemyNPCMob.h"

namespace SSB
{
	float EnemyNPCMob::GetBattleRange()
	{
		return m_BattleRange;
	}

	float EnemyNPCMob::GetSpotRange()
	{
		return m_SpotRange;
	}
	bool EnemyNPCMob::Init()
	{
		Character::Init();

		m_AttackBox.CreateOBBBox(0.5f, 1, 0.3f);
		m_AttackBoxLocalMatrix = TMatrix(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 100, -150, 1
		);

		m_Damage = 10;

		return true;
	}
}
