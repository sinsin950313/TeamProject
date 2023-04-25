#include "FieldBossStateService.h"
#include "FieldBoss.h"
#include "Player.h"
#include "CollisionMgr.h"

namespace SSB
{
    void FieldBossIdleState::StateDecision()
    {
        FieldBoss* mob = static_cast<FieldBoss*>(m_pCharacter);
		if (mob->IsDead())
		{
            ReserveNextTransferName(kFieldBossMobDead);
			SetTransfer();
		}

        if (mob->IsAirborne())
        {
            ReserveNextTransferName(kFieldBossMobAirborne);
			SetTransfer();
        }

		float fDistance = TVector3::Distance(Player::GetInstance().GetPosition(), mob->GetPosition());
        if (fDistance <= mob->GetBattleRange())
        {
            if (!(0 < m_pCharacter->GetRemainSkillCoolTime(kFieldBossMobSkillCasting)))
            {
                ReserveNextTransferName(kFieldBossMobSkillCasting);
                SetTransfer();
            }
        }

		if (fDistance <= mob->GetSpotRange())
		{
			ReserveNextTransferName(kFieldBossMobMove);
			SetTransfer();
		}

		if (fDistance <= mob->GetBattleRange())
		{
			ReserveNextTransferName(kFieldBossMobAttack);
			SetTransfer();
		}
    }
    void FieldBossIdleState::Action()
    {
    }
    std::vector<std::string> FieldBossIdleState::GetLinkedList()
    {
        return { kFieldBossMobDead, kFieldBossMobMove, kFieldBossMobAttack, kFieldBossMobAirborne, kFieldBossMobSkillCasting };
    }
    void FieldBossMoveState::StateDecision()
    {
		FieldBoss* mob = static_cast<FieldBoss*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
        if (m_pCharacter->IsDead())
        {
            ReserveNextTransferName(kFieldBossMobDead);
            SetTransfer();
		}

        if (mob->IsAirborne())
        {
            ReserveNextTransferName(kFieldBossMobAirborne);
			SetTransfer();
        }

        if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
        {
            if (!(0 < m_pCharacter->GetRemainSkillCoolTime(kFieldBossMobSkillCasting)))
            {
                ReserveNextTransferName(kFieldBossMobSkillCasting);
                SetTransfer();
            }
        }

		if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
		{
            ReserveNextTransferName(kFieldBossMobAttack);
            SetTransfer();
		}

		if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
		{
            ReserveNextTransferName(kFieldBossMobIdle);
            SetTransfer();
		}
    }
    void FieldBossMoveState::Action()
    {
		XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
        FieldBoss* mob = static_cast<FieldBoss*>(m_pCharacter);
        XMVECTOR dir = Player::GetInstance().GetPosition() - m_pCharacter->GetPosition();;
        dir = XMVector3Normalize(dir);

        m_pCharacter->MoveChar(dir, world);
    }
    StateTransferPriority FieldBossMoveState::GetPriority()
    {
        return FieldBossMobMoveTypePriority;
    }
    std::vector<std::string> FieldBossMoveState::GetLinkedList()
    {
        return { kFieldBossMobDead, kFieldBossMobAttack, kFieldBossMobIdle, kFieldBossMobAirborne, kFieldBossMobSkillCasting };
    }
    FieldBossAttackState::FieldBossAttackState(float transferRequireTime) : _transferRequireTime(transferRequireTime)
    {
    }
    void FieldBossAttackState::StateDecision()
    {
        FieldBoss* mob = static_cast<FieldBoss*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
            ReserveNextTransferName(kFieldBossMobDead);
            SetTransfer();
		}

        if (mob->IsAirborne())
        {
            ReserveNextTransferName(kFieldBossMobAirborne);
			SetTransfer();
        }

        if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
        {
            if (!(0 < m_pCharacter->GetRemainSkillCoolTime(kFieldBossMobSkillCasting)))
            {
                ReserveNextTransferName(kFieldBossMobSkillCasting);
                SetTransfer();
            }
        }

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
			{
				ReserveNextTransferName(kFieldBossMobAttackReset);
				SetTransfer();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kFieldBossMobMove);
				SetTransfer();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kFieldBossMobIdle);
				SetTransfer();
			}
		}

        if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
        {
            //_blackboard->StateTImeStamp = g_fGameTimer;
            //_blackboard->DamagedCharacters.clear();
			ReserveNextTransferName(kFieldBossMobIdle);
			SetTransfer();
        }
    }
    void FieldBossAttackState::Action()
    {
        if (!_blackboard->Initialized)
        {
            _isAttacked = false;
        }

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

            //// Damage Timing ��
            //float time = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
            //if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > time)
            //{
            //    if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
            //    {
            //        Damage(_blackboard, &Player::GetInstance(), m_pCharacter->m_Damage);
            //        Player::GetInstance().m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage((float)Player::GetInstance().m_HealthPoint / Player::GetInstance().m_kHealthPointMax, 1.0f));
            //        Player::GetInstance().m_pInterDamageBlood->m_pWorkList.push_back(new InterfaceFadeOut(1.0f));
            //    }
            //}
        }

		float pivot = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.3f;
		if (pivot < m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime)
		{
            if (!_isAttacked)
            {
                static_cast<FieldBoss*>(m_pCharacter)->Attack();
                _isAttacked = true;
            }
		}
    }
    StateTransferPriority FieldBossAttackState::GetPriority()
    {
        return FieldBossMobAttackTypePriority;
    }
    float FieldBossAttackState::GetTransferRequireTime()
    {
        return _transferRequireTime;
    }
    std::vector<std::string> FieldBossAttackState::GetLinkedList()
    {
        return { kFieldBossMobDead, kFieldBossMobMove, kFieldBossMobIdle, kFieldBossMobAttackReset, kFieldBossMobAirborne, kFieldBossMobSkillCasting };
    }
    void FieldBossAttackReset::StateDecision()
    {
		if (m_pCharacter->IsDead())
		{
            ReserveNextTransferName(kFieldBossMobDead);
            SetTransfer();
		}

        if (m_pCharacter->IsAirborne())
        {
            ReserveNextTransferName(kFieldBossMobAirborne);
			SetTransfer();
        }

		ReserveNextTransferName(kFieldBossMobAttack);
		SetTransfer();
    }
    void FieldBossAttackReset::Action()
    {
    }
    StateTransferPriority FieldBossAttackReset::GetPriority()
    {
        return FieldBossMobAttackTypePriority;
    }
    std::vector<std::string> FieldBossAttackReset::GetLinkedList()
    {
        return { kFieldBossMobDead, kFieldBossMobAirborne, kFieldBossMobAttack };
    }
    FieldBossSkillCastingState::FieldBossSkillCastingState(float transferRequireTime) : _transferRequireTime(transferRequireTime)
    {
    }
    void FieldBossSkillCastingState::StateDecision()
    {
        FieldBoss* mob = static_cast<FieldBoss*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
            ReserveNextTransferName(kFieldBossMobDead);
            SetTransfer();
		}

        if (mob->IsAirborne())
        {
            ReserveNextTransferName(kFieldBossMobAirborne);
			SetTransfer();
        }

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kFieldBossMobSkillFire);
			SetTransfer();
		}
    }
    void FieldBossSkillCastingState::Action()
    {
        if (!_blackboard->Initialized)
        {
            m_pCharacter->ActiveSkill(kFieldBossMobSkillCasting);
        }

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
        }
    }
    StateTransferPriority FieldBossSkillCastingState::GetPriority()
    {
        return FieldBossMobSkillTypePriority;
    }
    float FieldBossSkillCastingState::GetTransferRequireTime()
    {
        return _transferRequireTime;
    }
    std::vector<std::string> FieldBossSkillCastingState::GetLinkedList()
    {
        return { kFieldBossMobDead, kFieldBossMobSkillFire, kFieldBossMobAirborne };
    }
    void FieldBossDeadState::StateDecision()
    {
    }
    void FieldBossDeadState::Action()
    {
        XMFLOAT4 tmpF(0, 0, 0, 0);
        XMVECTOR tmp = XMLoadFloat4(&tmpF);
		XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
		m_pCharacter->MoveChar(tmp, world);
    }
    StateTransferPriority FieldBossDeadState::GetPriority()
    {
        return FieldBossMobDeadTypePriority;
    }
    std::vector<std::string> FieldBossDeadState::GetLinkedList()
    {
        return std::vector<std::string>();
    }
    void FieldBossAirBorneState::StateDecision()
    {
        if (m_pCharacter->IsPoundState())
        {
			ReserveNextTransferName(kFieldBossMobPound);
			SetTransfer();
        }

        if(!m_pCharacter->IsAirborne())
		{
			ReserveNextTransferName(kFieldBossMobIdle);
			SetTransfer();
		}
    }
    void FieldBossAirBorneState::Action()
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
    StateTransferPriority FieldBossAirBorneState::GetPriority()
    {
        return FieldBossMobAirborneTypePriority;
    }
    std::vector<std::string> FieldBossAirBorneState::GetLinkedList()
    {
        return { kFieldBossMobIdle, kFieldBossMobPound };
    }
    FieldBossPoundState::FieldBossPoundState(float transferRequireTime) : _transferRequireTime(transferRequireTime)
    {
    }
    void FieldBossPoundState::StateDecision()
    {
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			XMVECTOR tmp;
			XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
			m_pCharacter->MoveChar(tmp, world);

			ReserveNextTransferName(kFieldBossMobIdle);
			SetTransfer();
		}
    }
    void FieldBossPoundState::Action()
    {
    }
    float FieldBossPoundState::GetTransferRequireTime()
    {
        return _transferRequireTime;
    }
    std::vector<std::string> FieldBossPoundState::GetLinkedList()
    {
        return { kFieldBossMobIdle };
    }
    FieldBossSkillFireState::FieldBossSkillFireState(float transferRequireTime) : _transferRequireTime(transferRequireTime)
    {
    }
    void FieldBossSkillFireState::StateDecision()
    {
        FieldBoss* mob = static_cast<FieldBoss*>(m_pCharacter);
		if (mob->IsDead())
		{
            ReserveNextTransferName(kFieldBossMobDead);
            SetTransfer();
		}

        if (mob->IsAirborne())
        {
            ReserveNextTransferName(kFieldBossMobAirborne);
			SetTransfer();
        }

        if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
        {
			ReserveNextTransferName(kFieldBossMobIdle);
			SetTransfer();
        }
    }
    void FieldBossSkillFireState::Action()
    {
		float pivot = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.3f;
		if (pivot < m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime)
		{
            if (_attackTimeStamp + _kAttackInterval < g_fGameTimer)
            {
                static_cast<FieldBoss*>(m_pCharacter)->Attack();
                _attackTimeStamp = g_fGameTimer;
            }
		}
    }
    StateTransferPriority FieldBossSkillFireState::GetPriority()
    {
        return FieldBossMobSkillTypePriority;
    }
    float FieldBossSkillFireState::GetTransferRequireTime()
    {
        return _transferRequireTime;
    }
    std::vector<std::string> FieldBossSkillFireState::GetLinkedList()
    {
        return { kFieldBossMobDead, kFieldBossMobAirborne, kFieldBossMobIdle };
    }
}
