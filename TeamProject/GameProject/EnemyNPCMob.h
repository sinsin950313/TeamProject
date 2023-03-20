#pragma once

#include "Character.h"

namespace SSB
{
    class EnemyNPCMob : public Character
    {
    public:
        float m_BattleRange = 30;
        float m_SpotRange = 300;

    public:
        float GetBattleRange();
        float GetSpotRange();
    };
}
