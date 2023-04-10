#pragma once

#include "Character.h"

namespace SSB
{
	class BossMob : public Character
	{
    public:
        float m_BattleRange = 5;
        float m_SpotRange = 30;
        float m_DashRange = 10;
        float m_SkillCoolTime = 5.0f;
		float m_fBeforeTime = 0;
        float m_LastSkillElapseTime = 0.0f;

        bool m_bDashCalled = false;
        bool m_bSkillCalled = false;
        TVector3 m_DashDirection;

    public:
        float GetBattleRange();
        float GetSpotRange();
        float GetDashRange();
        float GetLastSkillElapseTime();
        void ResetSkillElapseTime();

    public:
		bool Frame() override;
	};
}
