#include "BossStateService.h"
#include "BossMob.h"
#include "Player.h"
#include "CollisionMgr.h"

namespace SSB
{
	bool BossMobIdleState::IsTransfer()
	{
        bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (TVector3::Distance(Player::GetInstance().GetPosition(), mob->GetPosition()) <= mob->GetSpotRange())
			{
				transfer = true;
				SetNextTransferName(kBossMobMove);
			}
		}

        return transfer;
	}
	bool BossMobAngryState::IsTransfer()
	{
        bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (IsPassedRequireCoolTime(mob->GetStateElapseTime()))
			{
				transfer = true;
				SetNextTransferName(kBossMobIdle);
			}
		}

		return transfer;
	}
	bool BossMobSpawnState::IsTransfer()
	{
        bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (IsPassedRequireCoolTime(mob->GetStateElapseTime()))
			{
				transfer = true;
				SetNextTransferName(kBossMobAngry);
			}
		}

		return transfer;
	}
	bool BossMobStunState::IsTransfer()
	{
        bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (IsPassedRequireCoolTime(mob->GetStateElapseTime()))
			{
				transfer = true;
				SetNextTransferName(kBossMobSpawn);
			}
		}

		return transfer;
	}
	bool BossMobDeadState::IsTransfer()
	{
		return false;
	}
	bool BossMobMoveState::IsTransfer()
	{
		bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (m_pCharacter->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
			{
				transfer = true;
				SetNextTransferName(kBossMobAttack1);
			}

			if (mob->GetSkillCoolTime() < mob->GetLastSkillElapseTime())
			{
				if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetDashRange())
				{
					transfer = true;
					SetNextTransferName(kBossMobDashStart);
				}
			}

			if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
			{
				transfer = true;
				SetNextTransferName(kBossMobIdle);
			}
		}

		return transfer;
	}
	void BossMobMoveState::Run()
	{
		CharacterState::Run();
		XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		XMVECTOR dir = Player::GetInstance().GetPosition() - m_pCharacter->GetPosition();;
		dir = XMVector3Normalize(dir);

		m_pCharacter->MoveChar(dir, world);
	}
	bool BossMobDashStartState::IsTransfer()
	{
		bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);

		if (!mob->m_bDashCalled)
		{
			mob->m_bDashCalled = true;
			mob->ResetSkillElapseTime();

			XMVECTOR dir = Player::GetInstance().GetPosition() - m_pCharacter->GetPosition();;
			dir = XMVector3Normalize(dir);

			m_pCharacter->m_fSpeed = 20;
			XMStoreFloat3(&mob->m_DashDirection, dir);
		}

		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (IsPassedRequireCoolTime(mob->GetStateElapseTime()))
			{
				transfer = true;
				SetNextTransferName(kBossMobDash);
			}
		}

		return transfer;
	}
	bool BossMobDashEndState::IsTransfer()
	{
		bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (IsPassedRequireCoolTime(mob->GetStateElapseTime()))
			{
				transfer = true;
				SetNextTransferName(kBossMobIdle);

				mob->m_bSkillCalled = false;
				mob->m_bDashCalled = false;
			}
		}

		return transfer;
	}
	void BossMobDashEndState::Run()
	{
		CharacterState::Run();

		m_pCharacter->m_fSpeed = 15;

		if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
		{
			if (!m_pCharacter->IsAlreadyDamagedCurrentState(&Player::GetInstance()))
			{
				Player::GetInstance().Damage(m_pCharacter->m_Damage);
				m_pCharacter->DamagingCharacter(&Player::GetInstance());
			}
		}
	}
	bool BossMobAttack1State::IsTransfer()
	{
		bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (IsPassedRequireCoolTime(m_pCharacter->GetStateElapseTime()))
			{
				if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
				{
					transfer = true;
					SetNextTransferName(kBossMobMove);
				}
				else if (mob->GetSkillCoolTime() < mob->GetLastSkillElapseTime())
				{
					int ranVal = rand() % 2;

					if (ranVal == 0)
					{
						if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetDashRange())
						{
							transfer = true;
							SetNextTransferName(kBossMobDashStart);
						}
					}
					if(ranVal == 1)
					{
						transfer = true;
						SetNextTransferName(kBossMobSkill1);
					}
				}

				if (transfer == false)
				{
					if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
					{
						transfer = true;
						SetNextTransferName(kBossMobAttack2);
					}
				}

				if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
				{
					transfer = true;
					SetNextTransferName(kBossMobIdle);
				}
			}
		}

		return transfer;
	}
	void BossMobAttack1State::Run()
	{
		CharacterState::Run();

		// LookAt Target
		if (IsPassedRequireCoolTime(m_pCharacter->GetStateElapseTime()))
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
	bool BossMobAttack2State::IsTransfer()
	{
		bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (IsPassedRequireCoolTime(m_pCharacter->GetStateElapseTime()))
			{
				if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
				{
					transfer = true;
					SetNextTransferName(kBossMobMove);
				}
				else if (mob->GetSkillCoolTime() < mob->GetLastSkillElapseTime())
				{
					int ranVal = rand() % 2;

					if (ranVal == 0)
					{
						if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetDashRange())
						{
							transfer = true;
							SetNextTransferName(kBossMobDashStart);
						}
					}
					if (ranVal == 1)
					{
						transfer = true;
						SetNextTransferName(kBossMobSkill1);
					}
				}

				if (transfer == false)
				{
					if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
					{
						transfer = true;
						SetNextTransferName(kBossMobAttack1);
					}
				}

				if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
				{
					transfer = true;
					SetNextTransferName(kBossMobIdle);
				}
			}
		}

		return transfer;
	}
	void BossMobAttack2State::Run()
	{
		CharacterState::Run();

		// LookAt Target
		if (IsPassedRequireCoolTime(m_pCharacter->GetStateElapseTime()))
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
	bool BossMobSkill1State::IsTransfer()
	{
		bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		Character* targetPlayer = &Player::GetInstance();
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (IsPassedRequireCoolTime(m_pCharacter->GetStateElapseTime()))
			{
				if (TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()) <= mob->GetBattleRange())
				{
					transfer = true;
					SetNextTransferName(kBossMobAttack1);

					mob->m_bSkillCalled = false;
				}

				if (mob->GetBattleRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
				{
					transfer = true;
					SetNextTransferName(kBossMobMove);

					mob->m_bSkillCalled = false;
				}

				if (mob->GetSpotRange() < TVector3::Distance(targetPlayer->GetPosition(), mob->GetPosition()))
				{
					transfer = true;
					SetNextTransferName(kBossMobIdle);

					mob->m_bSkillCalled = false;
				}
			}
		}

		return transfer;
	}
	void BossMobSkill1State::Run()
	{
		CharacterState::Run();

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);

		if (!mob->m_bSkillCalled)
		{
			mob->m_bSkillCalled = true;
			mob->ResetSkillElapseTime();
		}

		// LookAt Target
		if (IsPassedRequireCoolTime(m_pCharacter->GetStateElapseTime()))
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
	bool BossMobDashState::IsTransfer()
	{
		bool transfer = false;

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (mob->IsDead())
		{
			transfer = true;
			SetNextTransferName(kBossMobDead);
		}
		else
		{
			if (IsPassedRequireCoolTime(mob->GetStateElapseTime()))
			{
				transfer = true;
				SetNextTransferName(kBossMobDashEnd);
			}
		}

		return transfer;
	}
	void BossMobDashState::Run()
	{
		CharacterState::Run();

		BossMob* mob = static_cast<BossMob*>(m_pCharacter);
		if (!mob->m_bSkillCalled)
		{
			mob->m_bSkillCalled = true;
			mob->ResetSkillElapseTime();
		}

		XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
		XMVECTOR dir = mob->m_DashDirection;
		m_pCharacter->MoveChar(dir, world);

		if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
		{
			if (!m_pCharacter->IsAlreadyDamagedCurrentState(&Player::GetInstance()))
			{
				Player::GetInstance().Damage(m_pCharacter->m_Damage);
				m_pCharacter->DamagingCharacter(&Player::GetInstance());
			}
		}
	}
}
