#include "PlayerStateService.h"
#include "Input.h"
#include "Player.h"
#include "CollisionMgr.h"

namespace SSB
{
	void PlayerIdleState::StateDecision()
	{
		if (m_pCharacter->IsDead())
		{
            ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		if (I_Input.GetKey(VK_SPACE) == KEY_HOLD)
		{
            ReserveNextTransferName(kPlayerDash);
			SetTransfer();
		}

		if (I_Input.GetKey('A') == KEY_HOLD ||
			I_Input.GetKey('D') == KEY_HOLD ||
			I_Input.GetKey('W') == KEY_HOLD ||
			I_Input.GetKey('S') == KEY_HOLD)
		{
            ReserveNextTransferName(kPlayerMove);
			SetTransfer();
		}

		if (I_Input.GetKey(VK_LBUTTON) == KEY_PUSH)
		{
            ReserveNextTransferName(kPlayerAttack1);
			SetTransfer();
		}
	}
	void PlayerIdleState::Action()
	{
	}
	std::vector<std::string> PlayerIdleState::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack1 };
	}
	void PlayerMoveState::StateDecision()
	{
		if (m_pCharacter->IsDead())
		{
            ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		if (I_Input.GetKey(VK_LBUTTON) == KEY_PUSH)
		{
            ReserveNextTransferName(kPlayerAttack1);
			SetTransfer();
		}

		if (I_Input.GetKey('A') != KEY_HOLD &&
			I_Input.GetKey('D') != KEY_HOLD &&
			I_Input.GetKey('W') != KEY_HOLD &&
			I_Input.GetKey('S') != KEY_HOLD)
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}
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
	std::vector<std::string> PlayerMoveState::GetLinkedList()
	{
		return { kPlayerDead, kPlayerAttack1, kPlayerIdle };
	}
	PlayerAttackState1::PlayerAttackState1(float transferRequireTime) : _transferRequireTime(transferRequireTime)
	{
	}
	void PlayerAttackState1::StateDecision()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		if (m_pCharacter->IsDead())
		{
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		if (0.5f < _blackboard->StateTImeStamp - g_fGameTimer)
		{
			if (I_Input.GetKey(VK_LBUTTON) == KEY_PUSH ||
				I_Input.GetKey(VK_LBUTTON) == KEY_HOLD)
			{
				ReserveNextTransferName(kPlayerAttack2);
			}
		}

		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
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
	float PlayerAttackState1::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> PlayerAttackState1::GetLinkedList()
	{
		return { kPlayerDead, kPlayerAttack2, kPlayerIdle };
	}
	PlayerAttackState2::PlayerAttackState2(float transferRequireTime) : _transferRequireTime(transferRequireTime)
	{
	}
	void PlayerAttackState2::StateDecision()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		if (m_pCharacter->IsDead())
		{
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		if (0.5f < _blackboard->StateTImeStamp - g_fGameTimer)
		{
			if (I_Input.GetKey(VK_LBUTTON) == KEY_PUSH ||
				I_Input.GetKey(VK_LBUTTON) == KEY_HOLD)
			{
				ReserveNextTransferName(kPlayerAttack3);
			}
		}

		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
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
					}
				}
			}
		}
	}
	StateTransferPriority PlayerAttackState2::GetPriority()
	{
		return PlayerAttackTypePriority;
	}
	float PlayerAttackState2::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> PlayerAttackState2::GetLinkedList()
	{
		return { kPlayerDead, kPlayerAttack3, kPlayerIdle };
	}
	PlayerAttackState3::PlayerAttackState3(float transferRequireTime) : _transferRequireTime(transferRequireTime)
	{
	}
	void PlayerAttackState3::StateDecision()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		if (m_pCharacter->IsDead())
		{
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		if (0.5f < _blackboard->StateTImeStamp - g_fGameTimer)
		{
			if (I_Input.GetKey(VK_LBUTTON) == KEY_PUSH ||
				I_Input.GetKey(VK_LBUTTON) == KEY_HOLD)
			{
				ReserveNextTransferName(kPlayerAttack4);
			}
		}

		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
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
	float PlayerAttackState3::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> PlayerAttackState3::GetLinkedList()
	{
		return { kPlayerDead, kPlayerAttack4, kPlayerIdle };
	}
	void PlayerDeadState::StateDecision()
	{
	}
	void PlayerDeadState::Action()
	{
	}
	StateTransferPriority PlayerDeadState::GetPriority()
	{
		return PlayerDeadTypePriority;
	}
	std::vector<std::string> PlayerDeadState::GetLinkedList()
	{
		return std::vector<std::string>();
	}
	PlayerAttackState4::PlayerAttackState4(float transferRequireTime) : _transferRequireTime(transferRequireTime)
	{
	}
	void PlayerAttackState4::StateDecision()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		if (m_pCharacter->IsDead())
		{
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		if (0.5f < _blackboard->StateTImeStamp - g_fGameTimer)
		{
			if (I_Input.GetKey(VK_LBUTTON) == KEY_PUSH ||
				I_Input.GetKey(VK_LBUTTON) == KEY_HOLD)
			{
				ReserveNextTransferName(kPlayerAttack4);
			}
		}

		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
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
					}
				}
			}
		}
	}
	StateTransferPriority PlayerAttackState4::GetPriority()
	{
		return PlayerAttackTypePriority;
	}
	float PlayerAttackState4::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> PlayerAttackState4::GetLinkedList()
	{
		return { kPlayerDead, kPlayerAttack1, kPlayerIdle };
	}
	PlayerSkillState1::PlayerSkillState1(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	void PlayerSkillState1::StateDecision()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		if (m_pCharacter->IsDead())
		{
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}
	}
	void PlayerSkillState1::Action()
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
	StateTransferPriority PlayerSkillState1::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	float PlayerSkillState1::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> PlayerSkillState1::GetLinkedList()
	{
		return { kPlayerDead, kPlayerIdle };
	}
	PlayerSkillState2::PlayerSkillState2(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	void PlayerSkillState2::StateDecision()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		if (m_pCharacter->IsDead())
		{
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}
	}
	void PlayerSkillState2::Action()
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
	StateTransferPriority PlayerSkillState2::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	float PlayerSkillState2::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> PlayerSkillState2::GetLinkedList()
	{
		return { kPlayerDead, kPlayerIdle };
	}
	PlayerSkillState3::PlayerSkillState3(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	void PlayerSkillState3::StateDecision()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		if (m_pCharacter->IsDead())
		{
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}
	}
	void PlayerSkillState3::Action()
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
	StateTransferPriority PlayerSkillState3::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	float PlayerSkillState3::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> PlayerSkillState3::GetLinkedList()
	{
		return { kPlayerDead, kPlayerIdle };
	}
	PlayerSkillState4::PlayerSkillState4(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	void PlayerSkillState4::StateDecision()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		if (m_pCharacter->IsDead())
		{
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}
	}
	void PlayerSkillState4::Action()
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
	StateTransferPriority PlayerSkillState4::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	float PlayerSkillState4::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> PlayerSkillState4::GetLinkedList()
	{
		return { kPlayerDead, kPlayerIdle };
	}
	PlayerSkillState5::PlayerSkillState5(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	void PlayerSkillState5::StateDecision()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		if (m_pCharacter->IsDead())
		{
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			ReserveNextTransferName(kPlayerDead);
			SetTransfer();
		}

		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}
	}
	void PlayerSkillState5::Action()
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
	StateTransferPriority PlayerSkillState5::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	float PlayerSkillState5::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> PlayerSkillState5::GetLinkedList()
	{
		return { kPlayerDead, kPlayerIdle };
	}
	PlayerDashState::PlayerDashState(float transferRequireTime)
	{
		_transferRequireTime = transferRequireTime;
	}
	void PlayerDashState::StateDecision()
	{
		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
			Player* player = static_cast<Player*>(m_pCharacter);
			if (player != nullptr)
			{
				player->m_IsImmortal = false;
			}
		}
	}
	void PlayerDashState::Action()
	{
	}
	StateTransferPriority PlayerDashState::GetPriority()
	{
		return PlayerDashTypePriority;
	}
	float PlayerDashState::GetTransferRequireTime()
	{
		return _transferRequireTime;
	}
	std::vector<std::string> PlayerDashState::GetLinkedList()
	{
		return { kPlayerIdle };
	}
}
