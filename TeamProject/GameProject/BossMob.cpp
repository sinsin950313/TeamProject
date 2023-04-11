#include "BossMob.h"

namespace SSB
{
	float BossMob::GetSkillCoolTime()
	{
		return 8;
	}
	float BossMob::GetBattleRange()
	{
		return m_BattleRange;
	}
	float BossMob::GetSpotRange()
	{
		return m_SpotRange;
	}
	float BossMob::GetDashRange()
	{
		return m_DashRange;
	}
	void BossMob::SetLastSkillTimeStamp()
	{
		m_LastSkillTimeStamp = g_fGameTimer;
	}
	float BossMob::GetLastSkillTimeStamp()
	{
		return m_LastSkillTimeStamp;
	}
	bool BossMob::Frame()
	{
		Character::Frame();

		return true;
	}
}
