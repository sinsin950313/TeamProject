#include "BossMob.h"

namespace SSB
{
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
	float BossMob::GetLastSkillElapseTime()
	{
		return m_LastSkillElapseTime;
	}
	void BossMob::ResetSkillElapseTime()
	{
		m_LastSkillElapseTime = 0.0f;
	}
	bool BossMob::Frame()
	{
		Character::Frame();

		float elapseTime = g_fGameTimer - m_fBeforeTime;
		m_LastSkillElapseTime += elapseTime;
		m_fBeforeTime = g_fGameTimer;

		return true;
	}
}
