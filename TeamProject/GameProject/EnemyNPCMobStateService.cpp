#include "EnemyNPCMobStateService.h"
#include "TMath.h"
#include "EnemyNPCMob.h"
#include "Player.h"
#include "CollisionMgr.h"
#include "SoundMgr.h"

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
                float fDistance = TVector3::Distance(Player::GetInstance().GetPosition(), mob->GetPosition());
				if (fDistance <= mob->GetSpotRange())
				{
					transfer = true;
					SetNextTransferName(kEnemyNPCMobMove);
				}

				if (fDistance <= mob->GetBattleRange())
				{
					transfer = true;
					SetNextTransferName(kEnemyNPCMobAttack);
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

            if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
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
                if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
                {
                    transfer = true;
                    SetNextTransferName(kEnemyNPCMobMove);
                }

                if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
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
        if(IsPassedRequireCoolTime(m_pCharacter->GetStateElapseTime()))
        {
            XMVECTOR oldCharDirection = m_pCharacter->m_vDirection;
            oldCharDirection = XMVector3Normalize(oldCharDirection);

            XMVECTOR destinationDirection = Player::GetInstance().GetPosition() - m_pCharacter->GetPosition();;
            destinationDirection = XMVector3Normalize(destinationDirection);

            if (XMVectorGetX(XMVector3Dot(destinationDirection, oldCharDirection)) == -1)
                oldCharDirection += XMVectorSet(0.4f, 0.0f, -0.4f, 0.0f);

            XMVECTOR currCharDirection = (oldCharDirection)+(destinationDirection);	// Get the characters direction (based off time, old position, and desired
            currCharDirection = XMVector3Normalize(currCharDirection);

            XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
            float charDirAngle = XMVectorGetX(XMVector3AngleBetweenNormals(XMVector3Normalize(currCharDirection), XMVector3Normalize(DefaultForward)));
            if (XMVectorGetY(XMVector3Cross(currCharDirection, DefaultForward)) > 0.0f)
                charDirAngle = -charDirAngle;

            m_pCharacter->m_vRotation = TVector3(0, charDirAngle - 3.14159265f, 0);

            // Set the characters old direction
            m_pCharacter->m_vDirection = TVector3(XMVectorGetX(currCharDirection), XMVectorGetY(currCharDirection), XMVectorGetZ(currCharDirection));

            TQuaternion q;
            D3DXQuaternionRotationYawPitchRoll(&q, m_pCharacter->m_vRotation.y, m_pCharacter->m_vRotation.x, m_pCharacter->m_vRotation.z);
            D3DXMatrixAffineTransformation(&m_pCharacter->m_matWorld, &m_pCharacter->m_vScale, nullptr, &q, &m_pCharacter->m_vPos);
        }

		if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
        {
            if (!m_pCharacter->IsAlreadyDamagedCurrentState(&Player::GetInstance()))
            {
                Player::GetInstance().Damage(m_pCharacter->m_Damage);
                m_pCharacter->DamagingCharacter(&Player::GetInstance());
            }
        }
    }
    bool EnemyNPCMobDeadState::IsTransfer()
    {
        return false;
    }
}
