#include "PlayerStateService.h"
#include "Input.h"
#include "Player.h"
#include "CollisionMgr.h"

namespace SSB
{
	PlayerStateCommonMethodInterface::PlayerStateCommonMethodInterface(float transferRequireTime) : _transferRequireTime(transferRequireTime)
	{
	}
	void PlayerStateCommonMethodInterface::Decision_IsDead()
	{
		if (m_pCharacter->IsDead())
		{
            ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}
	}
	void PlayerStateCommonMethodInterface::Decision_IsDash()
	{
		if (I_Input.GetKey(VK_SPACE) == KEY_HOLD && !(0 < m_pCharacter->GetRemainSkillCoolTime(kPlayerDash)))
		{
            ReserveNextTransferName(kPlayerDash);
			SetTransfer();
		}
	}
	void PlayerStateCommonMethodInterface::Decision_IsSkill(SSB::StateName skillStateName, char key, float minimumRequiredTime)
	{
		if (minimumRequiredTime < g_fGameTimer - _blackboard->StateTImeStamp)
		{
			if (I_Input.GetKey(key) == KEY_PUSH && !(0 < m_pCharacter->GetRemainSkillCoolTime(skillStateName)))
			{
				ReserveNextTransferName(skillStateName);
			}
		}
	}
	void PlayerStateCommonMethodInterface::Decision_IsAttack(SSB::StateName nextAttackStateName, float minimumRequiredTime)
	{
		if (minimumRequiredTime < g_fGameTimer - _blackboard->StateTImeStamp)
		{
			if (I_Input.GetKey(VK_LBUTTON) == KEY_PUSH ||
				I_Input.GetKey(VK_LBUTTON) == KEY_HOLD)
			{
				ReserveNextTransferName(nextAttackStateName);
			}
		}
	}
	void PlayerStateCommonMethodInterface::Decision_IsMove()
	{
		if (I_Input.GetKey('A') == KEY_HOLD ||
			I_Input.GetKey('D') == KEY_HOLD ||
			I_Input.GetKey('W') == KEY_HOLD ||
			I_Input.GetKey('S') == KEY_HOLD)
		{
            ReserveNextTransferName(kPlayerMove);
		}
	}
	void PlayerStateCommonMethodInterface::Decision_IsTransfer()
	{
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}
	}
	float PlayerStateCommonMethodInterface::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	void PlayerIdleState::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsMove();

		Decision_IsAttack(kPlayerAttack1, 0);

		Decision_IsSkill(kPlayerSkill1, 'Q', 0);

		Decision_IsSkill(kPlayerSkill2, 'E', 0);

		Decision_IsTransfer();
	}
	void PlayerIdleState::Action()
	{
	}
	std::vector<std::string> PlayerIdleState::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack1, kPlayerSkill1, kPlayerSkill2, kPlayerIdle };
	}
	void PlayerMoveState::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsMove();

		Decision_IsAttack(kPlayerAttack1, 0);

		Decision_IsSkill(kPlayerSkill1, 'Q', 0);

		Decision_IsSkill(kPlayerSkill2, 'E', 0);

		Decision_IsTransfer();
	}
	void PlayerMoveState::Action()
	{
		Player* player = static_cast<Player*>(m_pCharacter);

		bool moveChar = false;
		XMVECTOR desiredCharDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		if (I_Input.GetKey('A') == KEY_HOLD)
		{
			desiredCharDir += -(player->m_pMainCamera->m_vRight);
			moveChar = true;
		}
		if (I_Input.GetKey('D') == KEY_HOLD)
		{
			desiredCharDir += (player->m_pMainCamera->m_vRight);
			moveChar = true;
		}
		if (I_Input.GetKey('W') == KEY_HOLD)
		{
			desiredCharDir += (player->m_pMainCamera->m_vLook);
			moveChar = true;
		}
		if (I_Input.GetKey('S') == KEY_HOLD)
		{
			desiredCharDir += -(player->m_pMainCamera->m_vLook);
			moveChar = true;
		}

		if (moveChar == true)
		{
			XMMATRIX world = XMLoadFloat4x4(&player->m_matWorld);
			player->MoveChar(desiredCharDir, world);
		}
	}
	StateTransferPriority PlayerMoveState::GetPriority()
	{
		return PlayerMoveTypePriority;
	}
	std::vector<std::string> PlayerMoveState::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack1, kPlayerSkill1, kPlayerSkill2, kPlayerIdle };
	}
	PlayerAttackState1::PlayerAttackState1(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerAttackState1::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsMove();

		float minTime = 0.5f;
		Decision_IsAttack(kPlayerAttack2, minTime);

		Decision_IsSkill(kPlayerSkill1, 'Q', minTime);

		Decision_IsSkill(kPlayerSkill2, 'E', minTime);

		Decision_IsTransfer();

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;
		}
	}
	void PlayerAttackState1::Action()
	{
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			Player::GetInstance().m_pTrail->StartTrail(&Player::GetInstance().m_matWorld);
		}

		static float timer = 0.0f;
		timer += g_fSecondPerFrame;
		if (timer > 0.02f)
		{
			Player::GetInstance().m_pTrail->AddTrailPos(
				Player::GetInstance().GetCurSocketPos("WeaponLow"), Player::GetInstance().GetCurSocketPos("WeaponHigh"));
			timer = 0.0f;
		}

		// 선택된 소켓의 애니메이션 행렬을 가져와서 어택 박스에 적용시켜서 
		// 충돌 처리가 될 수 있게끔 해야함
		float time = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > time)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						Damage(_blackboard, obj, m_pCharacter->m_Damage);
					}
				}
			}
		}
	}
	StateTransferPriority PlayerAttackState1::GetPriority()
	{
		return PlayerAttackTypePriority;
	}
	std::vector<std::string> PlayerAttackState1::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack2, kPlayerSkill1, kPlayerSkill2, kPlayerIdle };
	}
	PlayerAttackState2::PlayerAttackState2(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerAttackState2::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsMove();

		float minTime = 0.5f;
		Decision_IsAttack(kPlayerAttack3, minTime);

		Decision_IsSkill(kPlayerSkill1, 'Q', minTime);

		Decision_IsSkill(kPlayerSkill2, 'E', minTime);

		Decision_IsTransfer();

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;
		}
	}
	void PlayerAttackState2::Action()
	{
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			Player::GetInstance().m_pTrail->StartTrail(&Player::GetInstance().m_matWorld);
		}

		static float timer = 0.0f;
		timer += g_fSecondPerFrame;
		if (timer > 0.02f)
		{
			Player::GetInstance().m_pTrail->AddTrailPos(
				Player::GetInstance().GetCurSocketPos("WeaponLow"), Player::GetInstance().GetCurSocketPos("WeaponHigh"));
			timer = 0.0f;
		}

		// 선택된 소켓의 애니메이션 행렬을 가져와서 어택 박스에 적용시켜서 
		// 충돌 처리가 될 수 있게끔 해야함
		float time = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > time)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						Damage(_blackboard, obj, m_pCharacter->m_Damage);
						obj->m_pGageHP->m_pWorkList.push_back(new InterfaceSetGage((float)obj->m_HealthPoint / obj->m_HealthPointMax, 1.0f));
						obj->m_pDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_pCharacter->m_Damage, obj->m_pDamage, 0.5f, 10.0f));
					}
				}
			}
		}
	}
	StateTransferPriority PlayerAttackState2::GetPriority()
	{
		return PlayerAttackTypePriority;
	}
	std::vector<std::string> PlayerAttackState2::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack3, kPlayerSkill1, kPlayerSkill2, kPlayerIdle };
	}
	PlayerAttackState3::PlayerAttackState3(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerAttackState3::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsMove();

		float minTime = 0.5f;
		Decision_IsAttack(kPlayerAttack4, minTime);

		Decision_IsSkill(kPlayerSkill1, 'Q', minTime);

		Decision_IsSkill(kPlayerSkill2, 'E', minTime);

		Decision_IsTransfer();

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;
		}
	}
	void PlayerAttackState3::Action()
	{
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			Player::GetInstance().m_pTrail->StartTrail(&Player::GetInstance().m_matWorld);
		}

		static float timer = 0.0f;
		timer += g_fSecondPerFrame;
		if (timer > 0.02f)
		{
			Player::GetInstance().m_pTrail->AddTrailPos(
				Player::GetInstance().GetCurSocketPos("WeaponLow"), Player::GetInstance().GetCurSocketPos("WeaponHigh"));
			timer = 0.0f;
		}

		// 선택된 소켓의 애니메이션 행렬을 가져와서 어택 박스에 적용시켜서 
		// 충돌 처리가 될 수 있게끔 해야함
		float time = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > time)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						Damage(_blackboard, obj, m_pCharacter->m_Damage);
					}
				}
			}
		}
	}
	StateTransferPriority PlayerAttackState3::GetPriority()
	{
		return PlayerAttackTypePriority;
	}
	std::vector<std::string> PlayerAttackState3::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack4, kPlayerSkill1, kPlayerSkill2, kPlayerIdle };
	}
	PlayerAttackState4::PlayerAttackState4(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerAttackState4::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsMove();

		float minTime = 0.5f;
		Decision_IsAttack(kPlayerAttack1, minTime);

		Decision_IsSkill(kPlayerSkill1, 'Q', minTime);

		Decision_IsSkill(kPlayerSkill2, 'E', minTime);

		Decision_IsTransfer();

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;
		}
	}
	void PlayerAttackState4::Action()
	{
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			Player::GetInstance().m_pTrail->StartTrail(&Player::GetInstance().m_matWorld);
		}

		static float timer = 0.0f;
		timer += g_fSecondPerFrame;
		if (timer > 0.02f)
		{
			Player::GetInstance().m_pTrail->AddTrailPos(
				Player::GetInstance().GetCurSocketPos("WeaponLow"), Player::GetInstance().GetCurSocketPos("WeaponHigh"));
			timer = 0.0f;
		}

		// 선택된 소켓의 애니메이션 행렬을 가져와서 어택 박스에 적용시켜서 
		// 충돌 처리가 될 수 있게끔 해야함
		float time = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > time)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						Damage(_blackboard, obj, m_pCharacter->m_Damage);
						obj->m_pGageHP->m_pWorkList.push_back(new InterfaceSetGage((float)obj->m_HealthPoint / obj->m_HealthPointMax, 1.0f));
						obj->m_pDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_pCharacter->m_Damage, obj->m_pDamage, 0.5f, 10.0f));
					}
				}
			}
		}
	}
	StateTransferPriority PlayerAttackState4::GetPriority()
	{
		return PlayerAttackTypePriority;
	}
	std::vector<std::string> PlayerAttackState4::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack1, kPlayerSkill1, kPlayerSkill2, kPlayerIdle };
	}
	PlayerSkillState1::PlayerSkillState1(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerSkillState1::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsTransfer();

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			Player::GetInstance().m_AttackBoxLocalMatrix = TMatrix(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 100, -200, 1
			);

			Player::GetInstance().m_AttackBox.fExtent[0] = 1;
			Player::GetInstance().m_AttackBox.fExtent[1] = 1;
			Player::GetInstance().m_AttackBox.fExtent[2] = 1;
		}
	}
	void PlayerSkillState1::Action()
	{
		if (!_blackboard->Initialized)
		{
			Player::GetInstance().ActiveSkill(kPlayerSkill1);

			Player::GetInstance().m_AttackBoxLocalMatrix = TMatrix(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			);

			Player::GetInstance().m_AttackBox.fExtent[0] = 2;
			Player::GetInstance().m_AttackBox.fExtent[1] = 2;
			Player::GetInstance().m_AttackBox.fExtent[2] = 2;
		}

		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			Player::GetInstance().m_pTrail->StartTrail(&Player::GetInstance().m_matWorld);
		}

		static float timer = 0.0f;
		timer += g_fSecondPerFrame;
		if (timer > 0.02f)
		{
			Player::GetInstance().m_pTrail->AddTrailPos(
				Player::GetInstance().GetCurSocketPos("WeaponLow"), Player::GetInstance().GetCurSocketPos("WeaponHigh"));
			timer = 0.0f;
		}

		// 선택된 소켓의 애니메이션 행렬을 가져와서 어택 박스에 적용시켜서 
		// 충돌 처리가 될 수 있게끔 해야함
		float time = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > time)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						Damage(_blackboard, obj, m_pCharacter->m_Damage * 0.5);
					}
				}
			}
		}
	}
	StateTransferPriority PlayerSkillState1::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	std::vector<std::string> PlayerSkillState1::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerIdle };
	}
	PlayerSkillState2::PlayerSkillState2(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerSkillState2::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsTransfer();

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			Player::GetInstance().m_AttackBoxLocalMatrix = TMatrix(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 100, -200, 1
			);

			Player::GetInstance().m_AttackBox.fExtent[0] = 1;
			Player::GetInstance().m_AttackBox.fExtent[1] = 1;
			Player::GetInstance().m_AttackBox.fExtent[2] = 1;
		}
	}
	void PlayerSkillState2::Action()
	{
		if (!_blackboard->Initialized)
		{
			Player::GetInstance().ActiveSkill(kPlayerSkill2);

			Player::GetInstance().m_AttackBoxLocalMatrix = TMatrix(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 100, -200, 1
			);

			Player::GetInstance().m_AttackBox.fExtent[0] = 0.5f;
			Player::GetInstance().m_AttackBox.fExtent[1] = 0.5f;
			Player::GetInstance().m_AttackBox.fExtent[2] = 2.0f;
		}

		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			Player::GetInstance().m_pTrail->StartTrail(&Player::GetInstance().m_matWorld);
		}

		static float timer = 0.0f;
		timer += g_fSecondPerFrame;
		if (timer > 0.02f)
		{
			Player::GetInstance().m_pTrail->AddTrailPos(
				Player::GetInstance().GetCurSocketPos("WeaponLow"), Player::GetInstance().GetCurSocketPos("WeaponHigh"));
			timer = 0.0f;
		}

		// 선택된 소켓의 애니메이션 행렬을 가져와서 어택 박스에 적용시켜서 
		// 충돌 처리가 될 수 있게끔 해야함
		float time = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > time)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						Damage(_blackboard, obj, m_pCharacter->m_Damage * 2);
						obj->m_pGageHP->m_pWorkList.push_back(new InterfaceSetGage((float)obj->m_HealthPoint / obj->m_HealthPointMax, 1.0f));
						obj->m_pDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_pCharacter->m_Damage, obj->m_pDamage, 0.5f, 10.0f));
					}
				}
			}
		}
	}
	StateTransferPriority PlayerSkillState2::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	std::vector<std::string> PlayerSkillState2::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerIdle };
	}
	PlayerDashState::PlayerDashState(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerDashState::StateDecision()
	{
		Decision_IsTransfer();

		if (IsTransfer())
		{
			Player* player = static_cast<Player*>(m_pCharacter);
			player->m_IsImmortal = false;
			player->m_fSpeed = 15;
			player->m_IsDash = false;
		}
	}
	void PlayerDashState::Action()
	{
		Player* player = static_cast<Player*>(m_pCharacter);
		player->m_IsDash = true;

		if (player != nullptr)
		{
			player->m_IsImmortal = true;
		}

		if (!_blackboard->Initialized)
		{
			player->ActiveSkill(kPlayerDash);

			XMVECTOR desiredCharDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

			if (I_Input.GetKey('A') == KEY_HOLD)
			{
				desiredCharDir += -(player->m_pMainCamera->m_vRight);
			}
			if (I_Input.GetKey('D') == KEY_HOLD)
			{
				desiredCharDir += (player->m_pMainCamera->m_vRight);
			}
			if (I_Input.GetKey('W') == KEY_HOLD)
			{
				desiredCharDir += (player->m_pMainCamera->m_vLook);
			}
			if (I_Input.GetKey('S') == KEY_HOLD)
			{
				desiredCharDir += -(player->m_pMainCamera->m_vLook);
			}

			auto tmp = XMVector3Length(desiredCharDir);
			XMFLOAT4 length;
			XMStoreFloat4(&length, tmp);
			if (length.x < 0.0001f)
			{
				// Model과 World의 방향이 반대로 되어있음
				desiredCharDir = -TVector4(player->m_matWorld._31, player->m_matWorld._32, player->m_matWorld._33, 0);
			}
			player->m_DashDirection = TVector3(XMVectorGetX(desiredCharDir), XMVectorGetY(desiredCharDir), XMVectorGetZ(desiredCharDir));
		}

		player->m_fSpeed = 20;
		XMMATRIX world = XMLoadFloat4x4(&player->m_matWorld);
		XMVECTOR dir = player->m_DashDirection;
		player->MoveChar(dir, world);
	}
	StateTransferPriority PlayerDashState::GetPriority()
	{
		return PlayerDashTypePriority;
	}
	std::vector<std::string> PlayerDashState::GetLinkedList()
	{
		return { kPlayerIdle };
	}
	void PlayerDeadState::StateDecision()
	{
	}
	void PlayerDeadState::Action()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		auto pTrail = Player::GetInstance().m_pTrail;
		for (int i = 0; i < pTrail->m_VertexList.size(); i++)
			pTrail->m_VertexList[i].c.w = 0.0f;
	}
	StateTransferPriority PlayerDeadState::GetPriority()
	{
		return PlayerDeadTypePriority;
	}
	std::vector<std::string> PlayerDeadState::GetLinkedList()
	{
		return std::vector<std::string>();
	}
}
