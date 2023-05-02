#include "BossMob.h"

namespace SSB
{
	float BossMob::GetSkillCoolTime()
	{
		return 8;
	}
	BossMob::BossMob() : Character(200)
	{
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
	bool BossMob::Init()
	{
		Character::Init();

		m_AttackBox.CreateOBBBox(2.0f, 2.0, 1.5f);
		m_AttackBoxLocalMatrix = TMatrix(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 200, -450, 1
		);
		//m_AttackBox.fExtent[0] = 2;
		//m_AttackBox.fExtent[1] = 2;
		//m_AttackBox.fExtent[2] = 1.5f;

		m_Damage = 30;

		return false;
	}
	bool BossMob::Frame()
	{
		Character::Frame();

		return true;
	}
}
