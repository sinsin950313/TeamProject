#pragma once

#include "Character.h"
#include "Camera.h"
namespace SSB
{
	class BossMob : public Character
	{
    public:
        Camera* m_pMainCamera;
    public:
        float m_BattleRange = 6;
        float m_SpotRange = 40;
        float m_DashRange = 30;
        float m_LastSkillTimeStamp;
        TVector3 m_DashDirection;

    public:
        BossMob();

    public:
        float GetSkillCoolTime();
        float GetBattleRange();
        float GetSpotRange();
        float GetDashRange();
        void SetLastSkillTimeStamp();
        float GetLastSkillTimeStamp();

    public:
		bool Init() override;
        bool Frame() override;
    public:
        virtual ~BossMob() {}

    public:
        bool _isAngry = false;
	};
}
