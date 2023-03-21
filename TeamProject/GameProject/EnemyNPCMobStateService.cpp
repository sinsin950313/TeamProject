#include "EnemyNPCMobStateService.h"
#include "TMath.h"
#include "EnemyNPCMob.h"
#include "Player.h"
#include "CollisionMgr.h"

namespace SSB
{
    bool EnemyNPCMobIdleState::IsTransfer()
    {
        bool transfer = false;

		EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kEnemyNPCMobDead);
		}
		else
		{
			if (IsPassedRequireCoolTime(mob->GetStateElapseTime()))
			{
				if (TVector3::Distance(Player::GetInstance().GetPosition(), mob->GetPosition()) <= mob->GetSpotRange())
				{
					transfer = true;
					SetNextTransferName(kEnemyNPCMobMove);
				}
			}
		}

        return transfer;
    }
    bool EnemyNPCMobMoveState::IsTransfer()
    {
        bool transfer = false;

		EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
        if (m_pCharacter->IsDead())
        {
			transfer = true;
			SetNextTransferName(kEnemyNPCMobDead);
        }
        else
        {
            if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
            {
                transfer = true;
                SetNextTransferName(kEnemyNPCMobAttack);
            }

            if (targetPlayer->IsDead())
            {
                transfer = true;
                SetNextTransferName(kEnemyNPCMobIdle);
            }
        }

        return transfer;
    }
    void EnemyNPCMobMoveState::Run()
    {
        CharacterState::Run();
		XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
        EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
        XMVECTOR dir = Player::GetInstance().GetPosition() - m_pCharacter->GetPosition();;
        dir = XMVector3Normalize(dir);

        m_pCharacter->MoveChar(dir, world);
    }
    bool EnemyNPCMobAttackState::IsTransfer()
    {
        bool transfer = false;

		EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kEnemyNPCMobDead);
		}
        else
        {
            if (IsPassedRequireCoolTime(m_pCharacter->GetStateElapseTime()))
            {
                if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
                {
                    transfer = true;
                    SetNextTransferName(kEnemyNPCMobIdle);
                }

                if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
                {
                    transfer = true;
                    SetNextTransferName(kEnemyNPCMobMove);
                }

                if (targetPlayer->IsDead())
                {
                    transfer = true;
                    SetNextTransferName(kEnemyNPCMobIdle);
                }
            }
        }

        return transfer;
    }
    void EnemyNPCMobAttackState::Run()
    {
        CharacterState::Run();
        // LookAt Target
        if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
        {
            Player::GetInstance().Damage(m_pCharacter->m_Damage, m_pCharacter->m_fStateTImeStamp);
        }
    }
    bool EnemyNPCMobDeadState::IsTransfer()
    {
        return false;
        std::string anim = "Attack" + std::to_string(rand() % 2 + 1);

        m_pCharacter->m_pModel->SetCurrentAnimation(anim);

		OutputDebugString(L"Enemy NPC Mob Attack\n");
    }
}
