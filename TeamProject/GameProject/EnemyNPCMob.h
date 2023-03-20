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
        Character* m_pTargetPlayer;

    public:
        float GetBattleRange();
        float GetSpotRange();

    public:
        void Initialize_SetTargetPlayer(Character* targetPlayer);
        Character* GetTargetPlayer();
    };
}
