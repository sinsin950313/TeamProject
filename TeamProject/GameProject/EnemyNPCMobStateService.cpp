#include "EnemyNPCMobStateService.h"
#include "TMath.h"
#include "EnemyNPCMob.h"
#include "Player.h"
#include "CollisionMgr.h"
#include "SoundMgr.h"

namespace SSB
{
    void EnemyNPCMobMoveState::StateDecision()
    {
		EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
        if (m_pCharacter->IsDead())
        {
            ReserveNextTransferName(kEnemyNPCMobDead);
            SetTransfer();
		}

        if (mob->IsAirborne())
        {
            ReserveNextTransferName(kEnemyNPCMobAirborne);
			SetTransfer();
        }

		if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
		{
            ReserveNextTransferName(kEnemyNPCMobAttack1);
            SetTransfer();
		}

		if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
		{
            ReserveNextTransferName(kEnemyNPCMobIdle);
            SetTransfer();
		}
    }
    void EnemyNPCMobMoveState::Action()
    {
		XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
        EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
        XMVECTOR dir = Player::GetInstance().GetPosition() - m_pCharacter->GetPosition();;
        dir = XMVector3Normalize(dir);

        m_pCharacter->MoveChar(dir, world);
    }
    StateTransferPriority EnemyNPCMobMoveState::GetPriority()
    {
        return EnemyNPCMobMoveTypePriority;
    }
    std::vector<std::string> EnemyNPCMobMoveState::GetLinkedList()
    {
        return { kEnemyNPCMobDead, kEnemyNPCMobAttack1, kEnemyNPCMobIdle, kEnemyNPCMobAirborne };
    }
    EnemyNPCMobAttack1State::EnemyNPCMobAttack1State(float transferRequireTime)
    {
        _transferRequireTime = transferRequireTime;
    }
    void EnemyNPCMobAttack1State::StateDecision()
    {
		EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
            ReserveNextTransferName(kEnemyNPCMobDead);
            SetTransfer();
		}

        if (mob->IsAirborne())
        {
            ReserveNextTransferName(kEnemyNPCMobAirborne);
			SetTransfer();
        }

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
			{
				ReserveNextTransferName(kEnemyNPCMobAttack2);
				SetTransfer();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kEnemyNPCMobMove);
				SetTransfer();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kEnemyNPCMobIdle);
				SetTransfer();
			}
		}

        if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
        {
            _blackboard->StateTImeStamp = g_fGameTimer;
            _blackboard->DamagedCharacters.clear();
        }
    }
    void EnemyNPCMobAttack1State::Action()
    {
        // LookAt Target
        if (!IsPassedRequiredTime(_blackboard->StateTImeStamp))
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

            // Damage Timing ��
            float time = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
            if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > time)
            {
                if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
                {
                    Damage(_blackboard, &Player::GetInstance(), m_pCharacter->m_Damage);
                }
            }
        }
    }
    StateTransferPriority EnemyNPCMobAttack1State::GetPriority()
    {
        return EnemyNPCMobAttackTypePriority;
    }
    float EnemyNPCMobAttack1State::GetTransferRequireTime()
    {
        return _transferRequireTime;
    }
    std::vector<std::string> EnemyNPCMobAttack1State::GetLinkedList()
    {
        return { kEnemyNPCMobDead, kEnemyNPCMobMove, kEnemyNPCMobIdle, kEnemyNPCMobAttack2, kEnemyNPCMobAirborne };
    }
    EnemyNPCMobAttack2State::EnemyNPCMobAttack2State(float transferRequireTime)
    {
        _transferRequireTime = transferRequireTime;
    }
    void EnemyNPCMobAttack2State::StateDecision()
    {
		EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
            ReserveNextTransferName(kEnemyNPCMobDead);
            SetTransfer();
		}

        if (mob->IsAirborne())
        {
            ReserveNextTransferName(kEnemyNPCMobAirborne);
			SetTransfer();
        }

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
			{
				ReserveNextTransferName(kEnemyNPCMobAttack1);
				SetTransfer();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kEnemyNPCMobMove);
				SetTransfer();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kEnemyNPCMobIdle);
				SetTransfer();
			}
		}

        if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
        {
            _blackboard->StateTImeStamp = g_fGameTimer;
            _blackboard->DamagedCharacters.clear();
        }
    }
    void EnemyNPCMobAttack2State::Action()
    {
        // LookAt Target
        if (!IsPassedRequiredTime(_blackboard->StateTImeStamp))
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

            // Damage Timing ��
            float time = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
            if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > time)
            {
                if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
                {
                    Damage(_blackboard, &Player::GetInstance(), m_pCharacter->m_Damage);
                    Player::GetInstance().m_pGageHP->m_pWorkList.push_back(new InterfaceSetGage((float)Player::GetInstance().m_HealthPoint / Player::GetInstance().m_HealthPointMax, 1.0f));
                }
            }
        }
    }
    StateTransferPriority EnemyNPCMobAttack2State::GetPriority()
    {
        return EnemyNPCMobAttackTypePriority;
    }
    float EnemyNPCMobAttack2State::GetTransferRequireTime()
    {
        return _transferRequireTime;
    }
    std::vector<std::string> EnemyNPCMobAttack2State::GetLinkedList()
    {
        return { kEnemyNPCMobDead, kEnemyNPCMobMove, kEnemyNPCMobIdle, kEnemyNPCMobAttack1, kEnemyNPCMobAirborne };
    }
    void EnemyNPCMobDeadState::StateDecision()
    {
    }
    void EnemyNPCMobDeadState::Action()
    {
    }
    StateTransferPriority EnemyNPCMobDeadState::GetPriority()
    {
        return EnemyNPCMobDeadTypePriority;
    }
    std::vector<std::string> EnemyNPCMobDeadState::GetLinkedList()
    {
        return std::vector<std::string>();
    }
    void EnemyNPCMobIdleState::StateDecision()
    {
		EnemyNPCMob* mob = static_cast<EnemyNPCMob*>(m_pCharacter);
		if (mob->IsDead())
		{
            ReserveNextTransferName(kEnemyNPCMobDead);
			SetTransfer();
		}

        if (mob->IsAirborne())
        {
            ReserveNextTransferName(kEnemyNPCMobAirborne);
			SetTransfer();
        }

		float fDistance = TVector3::Distance(Player::GetInstance().GetPosition(), mob->GetPosition());
		if (fDistance <= mob->GetSpotRange())
		{
			ReserveNextTransferName(kEnemyNPCMobMove);
			SetTransfer();
		}

		if (fDistance <= mob->GetBattleRange())
		{
			ReserveNextTransferName(kEnemyNPCMobAttack1);
			SetTransfer();
		}
    }
    void EnemyNPCMobIdleState::Action()
    {
    }
    std::vector<std::string> EnemyNPCMobIdleState::GetLinkedList()
    {
        return { kEnemyNPCMobDead, kEnemyNPCMobMove, kEnemyNPCMobAttack1, kEnemyNPCMobAirborne };
    }
    void EnemyNPCMobAirBorneState::StateDecision()
    {
        if (m_pCharacter->IsPoundState())
        {
			ReserveNextTransferName(kEnemyNPCMobPound);
			SetTransfer();
        }

        if(!m_pCharacter->IsAirborne())
		{
			ReserveNextTransferName(kEnemyNPCMobIdle);
			SetTransfer();
		}
    }
    void EnemyNPCMobAirBorneState::Action()
    {
        if (!m_pCharacter->IsPoundState())
        {
            float halfTransferTime = m_pCharacter->GetAirborneActiveTime() * 0.5f;
            float elapseTime = g_fGameTimer - _blackboard->StateTImeStamp;
            float height;
            if (elapseTime > halfTransferTime)
            {
                // Going Down
                height = _airborneHeight * (1.0f - (elapseTime - halfTransferTime) / halfTransferTime);
            }
            else
            {
                //Going Up
                height = _airborneHeight * (elapseTime / halfTransferTime);
            }
            m_pCharacter->m_vPos.y = height;
        }
    }
    StateTransferPriority EnemyNPCMobAirBorneState::GetPriority()
    {
        return EnemyNPCMobAirborneTypePriority;
    }
    std::vector<std::string> EnemyNPCMobAirBorneState::GetLinkedList()
    {
        return { kEnemyNPCMobIdle, kEnemyNPCMobPound };
    }
    EnemyNPCMobPoundState::EnemyNPCMobPoundState(float transferRequireTime) : _transferRequireTime(transferRequireTime)
    {
    }
    void EnemyNPCMobPoundState::StateDecision()
    {
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			XMVECTOR tmp;
			XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
			m_pCharacter->MoveChar(tmp, world);

			ReserveNextTransferName(kEnemyNPCMobIdle);
			SetTransfer();
		}
    }
    void EnemyNPCMobPoundState::Action()
    {
    }
    float EnemyNPCMobPoundState::GetTransferRequireTime()
    {
        return _transferRequireTime;
    }
    std::vector<std::string> EnemyNPCMobPoundState::GetLinkedList()
    {
        return { kEnemyNPCMobIdle };
    }
}
