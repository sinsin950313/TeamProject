#include "EnemyNPCMobStateService.h"
#include "TMath.h"
#include "EnemyNPCMob.h"
#include "Player.h"

namespace SSB
{
    bool EnemyNPCMobIdleState::IsTransfer()
    {
        bool transfer = false;

        EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
        if (TVector3::Distance(Player::GetInstance().GetPosition(), mob->GetPosition()) <= mob->GetSpotRange())
        {
			transfer = true;
			SetNextTransferName(kEnemyNPCMobMove);
        }

        if (mob->IsDead())
        {
			transfer = true;
			SetNextTransferName(kEnemyNPCMobDead);
        }

        return transfer;
    }
    void EnemyNPCMobIdleState::Run()
    {
        m_pCharacter->m_pModel->SetCurrentAnimation("Idle");
		OutputDebugString(L"Enemy NPC Mob Idle\n");
    }
    bool EnemyNPCMobMoveState::IsTransfer()
    {
        bool transfer = false;

        EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
        Character* targetPlayer = &Player::GetInstance();
        if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
        {
			transfer = true;
			SetNextTransferName(kEnemyNPCMobAttack);
        }

        if(targetPlayer->IsDead())
        {
			transfer = true;
			SetNextTransferName(kEnemyNPCMobIdle);
        }

        if (mob->IsDead())
        {
			transfer = true;
			SetNextTransferName(kEnemyNPCMobDead);
        }

        return transfer;
    }
    void EnemyNPCMobMoveState::Run()
    {
		XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
        EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
        XMVECTOR dir = Player::GetInstance().GetPosition() - m_pCharacter->GetPosition();;
        dir = XMVector3Normalize(dir);

        m_pCharacter->MoveChar(dir, world);

        m_pCharacter->m_pModel->SetCurrentAnimation("Move");
    }
    bool EnemyNPCMobAttackState::IsTransfer()
    {
        bool transfer = false;

        EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
        Character* targetPlayer = &Player::GetInstance();
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

        if(targetPlayer->IsDead())
        {
			transfer = true;
			SetNextTransferName(kEnemyNPCMobIdle);
        }

        if (mob->IsDead())
        {
			transfer = true;
			SetNextTransferName(kEnemyNPCMobDead);
        }

        return transfer;
    }
    void EnemyNPCMobAttackState::Run()
    {
        std::string anim = "Attack" + std::to_string(rand() % 2 + 1);

        m_pCharacter->m_pModel->SetCurrentAnimation(anim);

		OutputDebugString(L"Enemy NPC Mob Attack\n");
    }
}
