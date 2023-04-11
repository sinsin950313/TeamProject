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
        float m_LastSkillTimeStamp;
        TVector3 m_DashDirection;

    public:
        float GetSkillCoolTime();
        float GetBattleRange();
        float GetSpotRange();
        float GetDashRange();
        void SetLastSkillTimeStamp();
        float GetLastSkillTimeStamp();
	};
}
