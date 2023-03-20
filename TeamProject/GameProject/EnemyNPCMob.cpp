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
	void EnemyNPCMob::Initialize_SetTargetPlayer(Character* targetPlayer)
	{
		m_pTargetPlayer = targetPlayer;
	}
	Character* EnemyNPCMob::GetTargetPlayer()
	{
		return m_pTargetPlayer;
	}
}
