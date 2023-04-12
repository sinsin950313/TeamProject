#include "BossStateService.h"
#include "BossMob.h"
#include "Player.h"
#include "CollisionMgr.h"

namespace SSB
{
	void BossMobIdleState::StateDecision()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
            ReserveNextTransferName(kBossMobDead);
            SetTransfer();
		}

		if (TVector3::Distance(Player::GetInstance().GetPosition(), mob->GetPosition()) <= mob->GetSpotRange())
		{
            ReserveNextTransferName(kBossMobMove);
            SetTransfer();
		}
	}
	void BossMobIdleState::Action()
	{
	}
	std::vector<std::string> BossMobIdleState::GetLinkedList()
	{
		return { kBossMobDead, kBossMobMove };
	}
	BossMobAngryState::BossMobAngryState(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	void BossMobAngryState::StateDecision()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
            ReserveNextTransferName(kBossMobDead);
            SetTransfer();
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
            ReserveNextTransferName(kBossMobIdle);
            SetTransfer();
		}
	}
	void BossMobAngryState::Action()
	{
	}
	float BossMobAngryState::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> BossMobAngryState::GetLinkedList()
	{
		return { kBossMobDead, kBossMobIdle };
	}
	void BossMobSpawnState::StateDecision()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
            ReserveNextTransferName(kBossMobDead);
            SetTransfer();
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
            ReserveNextTransferName(kBossMobAngry);
            SetTransfer();
		}
	}
	void BossMobSpawnState::Action()
	{
	}
	//bool BossMobStunState::StateDecision()
	//{
 //       bool transfer = false;

	//	BossMob* mob = static_cast<BossMob*>(m_pCharacter);
	//	if (mob->IsDead())
	//	{
	//		transfer = true;
	//		SetNextTransferName(kBossMobDead);
	//	}
	//	else
	//	{
	//		if (IsPassedRequireCoolTime(mob->GetStateElapseTime()))
	//		{
	//			transfer = true;
	//			SetNextTransferName(kBossMobIdle);
	//		}
	//	}

	//	return transfer;
	//}
	void BossMobMoveState::StateDecision()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (m_pCharacter->IsDead())
		{
            ReserveNextTransferName(kBossMobDead);
            SetTransfer();
		}

		if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
		{
			ReserveNextTransferName(kBossMobAttack1);
			SetTransfer();
		}

		if (mob->GetSkillCoolTime() < g_fGameTimer - mob->GetLastSkillTimeStamp())
		{
			if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetDashRange())
			{
				ReserveNextTransferName(kBossMobDashStart);
				SetTransfer();
			}
		}

		if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
		{
			ReserveNextTransferName(kBossMobIdle);
			SetTransfer();
		}
	}
	void BossMobMoveState::Action()
	{
		XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		XMVECTOR dir = Player::GetInstance().GetPosition() - m_pCharacter->GetPosition();;
		dir = XMVector3Normalize(dir);

		m_pCharacter->MoveChar(dir, world);
	}
	std::vector<std::string> BossMobMoveState::GetLinkedList()
	{
		return { kBossMobDead, kBossMobAttack1, kBossMobDashStart, kBossMobIdle };
	}
	void BossMobDashStartState::StateDecision()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);

		if (!_blackboard->Initialized)
		{
			_blackboard->Initialized = true;

			XMVECTOR dir = Player::GetInstance().GetPosition() - m_pCharacter->GetPosition();;
			dir = XMVector3Normalize(dir);

			XMStoreFloat3(&mob->m_DashDirection, dir);
		}

		if (mob->IsDead())
		{
            ReserveNextTransferName(kBossMobDead);
            SetTransfer();
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kBossMobDash);
			SetTransfer();
		}
	}
	void BossMobDashStartState::Action()
	{
	}
	void BossMobDashEndState::StateDecision()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
            ReserveNextTransferName(kBossMobDead);
            SetTransfer();
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
            ReserveNextTransferName(kBossMobIdle);
            SetTransfer();

			mob->SetLastSkillTimeStamp();
		}
	}
	void BossMobDashEndState::Action()
	{
		if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
		{
			Damage(_blackboard, &Player::GetInstance(), m_pCharacter->m_Damage);
		}
	}
	void BossMobAttack1State::StateDecision()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
            ReserveNextTransferName(kBossMobDead);
            SetTransfer();
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kBossMobMove);
				SetTransfer();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetSkillCoolTime() < g_fGameTimer - mob->GetLastSkillTimeStamp())
			{
				int ranVal = rand() % 2;

				if (ranVal == 0)
				{
					if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetDashRange())
					{
						ReserveNextTransferName(kBossMobDashStart);
						SetTransfer();
					}
				}
				if (ranVal == 1)
				{
					ReserveNextTransferName(kBossMobSkill1);
					SetTransfer();
				}
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
			{
				ReserveNextTransferName(kBossMobAttack2);
				SetTransfer();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kBossMobIdle);
				SetTransfer();
			}
		}
	}
	void BossMobAttack1State::Action()
	{
		// LookAt Target
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
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
			Damage(_blackboard, &Player::GetInstance(), m_pCharacter->m_Damage);
		}
	}
	StateTransferPriority BossMobAttack1State::GetPriority()
	{
		return BossAttackTypePriority;
	}
	void BossMobAttack2State::StateDecision()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
            ReserveNextTransferName(kBossMobDead);
            SetTransfer();
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kBossMobMove);
				SetTransfer();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetSkillCoolTime() < g_fGameTimer - mob->GetLastSkillTimeStamp())
			{
				int ranVal = rand() % 2;

				if (ranVal == 0)
				{
					if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetDashRange())
					{
						ReserveNextTransferName(kBossMobDashStart);
						SetTransfer();
					}
				}
				if (ranVal == 1)
				{
					ReserveNextTransferName(kBossMobSkill1);
					SetTransfer();
				}
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
			{
				ReserveNextTransferName(kBossMobAttack1);
				SetTransfer();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kBossMobIdle);
				SetTransfer();
			}
		}
	}
	BossMobAttack1State::BossMobAttack1State(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	float BossMobAttack1State::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> BossMobAttack1State::GetLinkedList()
	{
		return { kBossMobDead, kBossMobMove, kBossMobDashStart, kBossMobSkill1, kBossMobAttack2, kBossMobIdle };
	}
	void BossMobAttack2State::Action()
	{
		// LookAt Target
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
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
			Damage(_blackboard, &Player::GetInstance(), m_pCharacter->m_Damage);
		}
	}
	StateTransferPriority BossMobAttack2State::GetPriority()
	{
		return BossAttackTypePriority;
	}
	void BossMobSkill1State::StateDecision()
	{
		bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
			ReserveNextTransferName(kBossMobDead);
			SetTransfer();
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
			{
				ReserveNextTransferName(kBossMobAttack1);
				SetTransfer();

				mob->SetLastSkillTimeStamp();
			}
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kBossMobMove);
				SetTransfer();

				mob->SetLastSkillTimeStamp();
			}
		}


		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				ReserveNextTransferName(kBossMobIdle);
				SetTransfer();

				mob->SetLastSkillTimeStamp();
			}
		}
	}
	void BossMobSkill1State::Action()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);

		// LookAt Target
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
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
			Damage(_blackboard, &Player::GetInstance(), m_pCharacter->m_Damage);
		}
	}
	StateTransferPriority BossMobSkill1State::GetPriority()
	{
		return BossSkillTypePriority;
	}
	void BossMobDashState::StateDecision()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
			ReserveNextTransferName(kBossMobDead);
			SetTransfer();
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kBossMobDashEnd);
			m_pCharacter->m_fSpeed = 15;
			SetTransfer();
		}
	}
	void BossMobDashState::Action()
	{
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);

		mob->m_fSpeed = 20;

		XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
		XMVECTOR dir = mob->m_DashDirection;
		m_pCharacter->MoveChar(dir, world);

		if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
		{
			Damage(_blackboard, &Player::GetInstance(), m_pCharacter->m_Damage);
		}
	}
	void BossMobDeadState::StateDecision()
	{
	}
	void BossMobDeadState::Action()
	{
	}
	StateTransferPriority BossMobDeadState::GetPriority()
	{
		return BossDeadTypePriority;
	}
	std::vector<std::string> BossMobDeadState::GetLinkedList()
	{
		return std::vector<std::string>();
	}
	BossMobAttack2State::BossMobAttack2State(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	float BossMobAttack2State::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> BossMobAttack2State::GetLinkedList()
	{
		return { kBossMobDead, kBossMobMove, kBossMobDashStart, kBossMobSkill1, kBossMobAttack1, kBossMobIdle };
	}
	BossMobDashStartState::BossMobDashStartState(float transferRequireTime)
	{
		_transferRequireTime = _transferRequireTime;
	}
	StateTransferPriority BossMobDashStartState::GetPriority()
	{
		return BossSkillTypePriority;
	}
	float BossMobDashStartState::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> BossMobDashStartState::GetLinkedList()
	{
		return { kBossMobDead, kBossMobDash };
	}
	BossMobDashState::BossMobDashState(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	float BossMobDashState::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> BossMobDashState::GetLinkedList()
	{
		return { kBossMobDead, kBossMobDashEnd };
	}
	BossMobDashEndState::BossMobDashEndState(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	float BossMobDashEndState::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> BossMobDashEndState::GetLinkedList()
	{
		return { kBossMobDead, kBossMobIdle };
	}
	BossMobSkill1State::BossMobSkill1State(float transferRequireTime)
	{
		_transferRequireTime = _transferRequireTime;
	}
	float BossMobSkill1State::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> BossMobSkill1State::GetLinkedList()
	{
		return { kBossMobDead, kBossMobAttack1, kBossMobMove, kBossMobIdle };
	}
	BossMobSpawnState::BossMobSpawnState(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	float BossMobSpawnState::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> BossMobSpawnState::GetLinkedList()
	{
		return { kBossMobDead, kBossMobAngry };
	}
}
