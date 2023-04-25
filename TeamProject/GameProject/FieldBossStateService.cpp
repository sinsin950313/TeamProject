#include "FieldBossStateService.h"
#include "FieldBoss.h"
#include "Player.h"
#include "CollisionMgr.h"

namespace SSB
{
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
        return { kFieldBossMobDead, kFieldBossMobAttack, kFieldBossMobIdle, kFieldBossMobAirborne };
    }
    FieldBossAttackState::FieldBossAttackState(float transferRequireTime)
    {
        _transferRequireTime = transferRequireTime;
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

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
			{
				ReserveNextTransferName(kFieldBossMobAttack);
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
            _blackboard->StateTImeStamp = g_fGameTimer;
            _blackboard->DamagedCharacters.clear();
        }
    }
    void FieldBossAttackState::Action()
    {
        if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
        {
            static_cast<FieldBoss*>(m_pCharacter)->Attack();
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

            // Damage Timing ��
            float time = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
            if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > time)
            {
                if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
                {
                    Damage(_blackboard, &Player::GetInstance(), m_pCharacter->m_Damage);
                    Player::GetInstance().m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage((float)Player::GetInstance().m_HealthPoint / Player::GetInstance().m_kHealthPointMax, 1.0f));
                    Player::GetInstance().m_pInterDamageBlood->m_pWorkList.push_back(new InterfaceFadeOut(1.0f));
                }
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
        return { kFieldBossMobDead, kFieldBossMobMove, kFieldBossMobIdle, kFieldBossMobAttack, kFieldBossMobAirborne };
    }
    FieldBossSkillState::FieldBossSkillState(float transferRequireTime)
    {
        _transferRequireTime = transferRequireTime;
    }
    void FieldBossSkillState::StateDecision()
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
			if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
			{
				ReserveNextTransferName(kFieldBossMobAttack);
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
            _blackboard->StateTImeStamp = g_fGameTimer;
            _blackboard->DamagedCharacters.clear();
        }
    }
    void FieldBossSkillState::Action()
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
                    Player::GetInstance().m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage((float)Player::GetInstance().m_HealthPoint / Player::GetInstance().m_kHealthPointMax, 1.0f));
                    Player::GetInstance().m_pInterDamageBlood->m_pWorkList.push_back(new InterfaceFadeOut(1.0f));
                }
            }
        }
    }
    StateTransferPriority FieldBossSkillState::GetPriority()
    {
        return FieldBossMobSkillTypePriority;
    }
    float FieldBossSkillState::GetTransferRequireTime()
    {
        return _transferRequireTime;
    }
    std::vector<std::string> FieldBossSkillState::GetLinkedList()
    {
        return { kFieldBossMobDead, kFieldBossMobMove, kFieldBossMobIdle, kFieldBossMobAttack, kFieldBossMobAirborne };
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
        return { kFieldBossMobDead, kFieldBossMobMove, kFieldBossMobAttack, kFieldBossMobAirborne };
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
}
