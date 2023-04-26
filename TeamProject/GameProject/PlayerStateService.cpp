#include "PlayerStateService.h"
#include "Input.h"
#include "Player.h"
#include "CollisionMgr.h"
#include "CameraTPS.h"
#include "SoundMgr.h"

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

		Decision_IsSkill(kPlayerPierce, 'Q', 0);

		Decision_IsSkill(kPlayerRotate, 'E', 0);

		Decision_IsSkill(kPlayerDrink, 'F', 0);

		if (Player::GetInstance().IsUlitmateSkillCallable())
		{
			Decision_IsSkill(kPlayerUltimate, 'R', 0);
		}

		if (Player::GetInstance().IsVictory())
		{
			ReserveNextTransferName(kPlayerVictoryStart);
			SetTransfer();
		}

		Decision_IsTransfer();
	}
	void PlayerIdleState::Action()
	{
	}
	std::vector<std::string> PlayerIdleState::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack1, kPlayerPierce, kPlayerRotate, kPlayerIdle, kPlayerUltimate, kPlayerDrink, kPlayerVictoryStart };
	}
	void PlayerMoveState::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsMove();

		Decision_IsAttack(kPlayerAttack1, 0);

		Decision_IsSkill(kPlayerPierce, 'Q', 0);

		Decision_IsSkill(kPlayerRotate, 'E', 0);

		Decision_IsSkill(kPlayerDrink, 'F', 0);

		if (Player::GetInstance().IsUlitmateSkillCallable())
		{
			Decision_IsSkill(kPlayerUltimate, 'R', 0);
		}

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
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack1, kPlayerPierce, kPlayerRotate, kPlayerIdle, kPlayerUltimate, kPlayerDrink };
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

		Decision_IsSkill(kPlayerPierce, 'Q', minTime);

		Decision_IsSkill(kPlayerRotate, 'E', minTime);

		Decision_IsTransfer();

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
			{
				//pTrail->m_VertexList[i].c.w = 0.0f;
				//pTrail->m_VertexCatmullRomList[i].p = TVector3(0, 0, 0);
			}
		}
	}
	void PlayerAttackState1::Action()
	{
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			std::wstring szRandomSound;
			switch ((int)randf(g_fGameTimer) % 4)
			{
			case 0:
				szRandomSound = L"yasuo_sound_attack1.mp3";
				break;
			case 1:
				szRandomSound = L"yasuo_sound_attack2.mp3";
				break;
			case 2:
				szRandomSound = L"yasuo_sound_attack3.mp3";
				break;
			case 3:
				szRandomSound = L"yasuo_sound_attack4.mp3";
				break;
			}
			auto sound = I_Sound.Find(szRandomSound);
			sound->VolumeSet(0.2f);
			sound->Play();
			Player::GetInstance().m_pTrail->ResetTrail(&Player::GetInstance().m_matWorld);
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
		float startTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		float endTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.3f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > startTime &&
			m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < endTime
			)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						if (_blackboard->DamagedCharacters.find(obj) == _blackboard->DamagedCharacters.end())
						{
							std::wstring szRandomSound;
							switch ((int)randf(g_fGameTimer) % 2)
							{
							case 0:
								szRandomSound = L"sound_dmg_player1.mp3";
								break;
							case 1:
								szRandomSound = L"sound_dmg_player2.mp3";
								break;
							}
							auto sound = I_Sound.Find(szRandomSound);
							sound->VolumeSet(0.2f);
							sound->Play();
							float currentHp = obj->m_HealthPoint - m_pCharacter->m_Damage;
							if (currentHp <= 0)
								currentHp = 0;
							obj->m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage(currentHp / obj->m_kHealthPointMax, 1.0f));
							obj->m_pInterDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_pCharacter->m_Damage, obj->m_pInterDamage, 0.5f, 10.0f));
						}
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
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack2, kPlayerPierce, kPlayerRotate, kPlayerIdle };
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

		Decision_IsSkill(kPlayerPierce, 'Q', minTime);

		Decision_IsSkill(kPlayerRotate, 'E', minTime);

		Decision_IsTransfer();

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
			{
				//pTrail->m_VertexList[i].c.w = 0.0f;
				//pTrail->m_VertexCatmullRomList[i].p = TVector3(0, 0, 0);
			}
		}
	}
	void PlayerAttackState2::Action()
	{
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			std::wstring szRandomSound;
			switch ((int)randf(g_fGameTimer) % 4)
			{
			case 0:
				szRandomSound = L"yasuo_sound_attack1.mp3";
				break;
			case 1:
				szRandomSound = L"yasuo_sound_attack2.mp3";
				break;
			case 2:
				szRandomSound = L"yasuo_sound_attack3.mp3";
				break;
			case 3:
				szRandomSound = L"yasuo_sound_attack4.mp3";
				break;
			}
			auto sound = I_Sound.Find(szRandomSound);
			sound->VolumeSet(0.2f);
			sound->Play();
			Player::GetInstance().m_pTrail->ResetTrail(&Player::GetInstance().m_matWorld);
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
		float startTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		float endTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.3f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > startTime &&
			m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < endTime
			)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						if (_blackboard->DamagedCharacters.find(obj) == _blackboard->DamagedCharacters.end())
						{
							std::wstring szRandomSound;
							switch ((int)randf(g_fGameTimer) % 2)
							{
							case 0:
								szRandomSound = L"sound_dmg_player1.mp3";
								break;
							case 1:
								szRandomSound = L"sound_dmg_player2.mp3";
								break;
							}
							auto sound = I_Sound.Find(szRandomSound);
							sound->VolumeSet(0.2f);
							sound->Play();
							float currentHp = obj->m_HealthPoint - m_pCharacter->m_Damage;
							if (currentHp <= 0)
								currentHp = 0;
							obj->m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage(currentHp / obj->m_kHealthPointMax, 1.0f));
							obj->m_pInterDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_pCharacter->m_Damage, obj->m_pInterDamage, 0.5f, 10.0f));
						}
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
	std::vector<std::string> PlayerAttackState2::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack3, kPlayerPierce, kPlayerRotate, kPlayerIdle };
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

		Decision_IsSkill(kPlayerPierce, 'Q', minTime);

		Decision_IsSkill(kPlayerRotate, 'E', minTime);

		Decision_IsTransfer();

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
			{
				//pTrail->m_VertexList[i].c.w = 0.0f;
				//pTrail->m_VertexCatmullRomList[i].p = TVector3(0, 0, 0);
			}
		}
	}
	void PlayerAttackState3::Action()
	{
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			std::wstring szRandomSound;
			switch ((int)randf(g_fGameTimer) % 4)
			{
			case 0:
				szRandomSound = L"yasuo_sound_attack1.mp3";
				break;
			case 1:
				szRandomSound = L"yasuo_sound_attack2.mp3";
				break;
			case 2:
				szRandomSound = L"yasuo_sound_attack3.mp3";
				break;
			case 3:
				szRandomSound = L"yasuo_sound_attack4.mp3";
				break;
			}
			auto sound = I_Sound.Find(szRandomSound);
			sound->VolumeSet(0.2f);
			sound->Play();
			Player::GetInstance().m_pTrail->ResetTrail(&Player::GetInstance().m_matWorld);
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
		float startTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		float endTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.3f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > startTime &&
			m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < endTime
			)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						if (_blackboard->DamagedCharacters.find(obj) == _blackboard->DamagedCharacters.end())
						{
							std::wstring szRandomSound;
							switch ((int)randf(g_fGameTimer) % 2)
							{
							case 0:
								szRandomSound = L"sound_dmg_player1.mp3";
								break;
							case 1:
								szRandomSound = L"sound_dmg_player2.mp3";
								break;
							}
							auto sound = I_Sound.Find(szRandomSound);
							sound->VolumeSet(0.2f);
							sound->Play();
							float currentHp = obj->m_HealthPoint - m_pCharacter->m_Damage;
							if (currentHp <= 0)
								currentHp = 0;
							obj->m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage(currentHp / obj->m_kHealthPointMax, 1.0f));
							obj->m_pInterDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_pCharacter->m_Damage, obj->m_pInterDamage, 0.5f, 10.0f));
						}
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
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack4, kPlayerPierce, kPlayerRotate, kPlayerIdle };
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

		Decision_IsSkill(kPlayerPierce, 'Q', minTime);

		Decision_IsSkill(kPlayerRotate, 'E', minTime);

		Decision_IsTransfer();

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
			{
				//pTrail->m_VertexList[i].c.w = 0.0f;
				//pTrail->m_VertexCatmullRomList[i].p = TVector3(0, 0, 0);
			}
		}
	}
	void PlayerAttackState4::Action()
	{
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			std::wstring szRandomSound;
			switch ((int)randf(g_fGameTimer) % 4)
			{
			case 0:
				szRandomSound = L"yasuo_sound_attack1.mp3";
				break;
			case 1:
				szRandomSound = L"yasuo_sound_attack2.mp3";
				break;
			case 2:
				szRandomSound = L"yasuo_sound_attack3.mp3";
				break;
			case 3:
				szRandomSound = L"yasuo_sound_attack4.mp3";
				break;
			}
			auto sound = I_Sound.Find(szRandomSound);
			sound->VolumeSet(0.2f);
			sound->Play();
			Player::GetInstance().m_pTrail->ResetTrail(&Player::GetInstance().m_matWorld);
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
		float startTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		float endTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.3f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > startTime &&
			m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < endTime
			)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						if (_blackboard->DamagedCharacters.find(obj) == _blackboard->DamagedCharacters.end())
						{
							std::wstring szRandomSound;
							switch ((int)randf(g_fGameTimer) % 2)
							{
							case 0:
								szRandomSound = L"sound_dmg_player1.mp3";
								break;
							case 1:
								szRandomSound = L"sound_dmg_player2.mp3";
								break;
							}
							auto sound = I_Sound.Find(szRandomSound);
							sound->VolumeSet(0.2f);
							sound->Play();
							float currentHp = obj->m_HealthPoint - m_pCharacter->m_Damage;
							if (currentHp <= 0)
								currentHp = 0;
							obj->m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage(currentHp / obj->m_kHealthPointMax, 1.0f));
							obj->m_pInterDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_pCharacter->m_Damage, obj->m_pInterDamage, 0.5f, 10.0f));
						}
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
	std::vector<std::string> PlayerAttackState4::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerMove, kPlayerAttack1, kPlayerPierce, kPlayerRotate, kPlayerIdle };
	}
	PlayerSkillRotate::PlayerSkillRotate(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerSkillRotate::StateDecision()
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
			{
				//pTrail->m_VertexList[i].c.w = 0.0f;
				//pTrail->m_VertexCatmullRomList[i].p = TVector3(0, 0, 0);
			}

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
	void PlayerSkillRotate::Action()
	{
		
		if (!_blackboard->Initialized)
		{
			Player::GetInstance().ActiveSkill(kPlayerRotate);

			Player::GetInstance().m_AttackBoxLocalMatrix = TMatrix(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			);

			Player::GetInstance().m_AttackBox.fExtent[0] = 2;
			Player::GetInstance().m_AttackBox.fExtent[1] = 2;
			Player::GetInstance().m_AttackBox.fExtent[2] = 2;
			Player::GetInstance().m_pInterSkillE->m_pWorkList.push_back(new InterfaceFadeClockwise(Player::GetInstance().GetSkillCoolTime(kPlayerRotate)));
		}

		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			std::wstring szRandomSound;
			switch ((int)randf(g_fGameTimer) % 6)
			{
			case 0:
				szRandomSound = L"yasuo_sound_attack1.mp3";
				break;
			case 1:
				szRandomSound = L"yasuo_sound_attack2.mp3";
				break;
			case 2:
				szRandomSound = L"yasuo_sound_attack3.mp3";
				break;
			case 3:
				szRandomSound = L"yasuo_sound_don.mp3";
				break;
			case 4:
				szRandomSound = L"yasuo_sound_hasegi1.mp3";
				break;
			case 5:
				szRandomSound = L"yasuo_sound_sul.mp3";
				break;
			}
			auto sound = I_Sound.Find(szRandomSound);
			sound->VolumeSet(0.2f);
			sound->Play();
			Player::GetInstance().m_pTrail->ResetTrail(&Player::GetInstance().m_matWorld);
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
		float startTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		float endTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.4f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > startTime &&
			m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < endTime
			)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						if (_blackboard->DamagedCharacters.find(obj) == _blackboard->DamagedCharacters.end())
						{
							std::wstring szRandomSound;
							switch ((int)randf(g_fGameTimer) % 2)
							{
							case 0:
								szRandomSound = L"sound_dmg_player1.mp3";
								break;
							case 1:
								szRandomSound = L"sound_dmg_player2.mp3";
								break;
							}
							auto sound = I_Sound.Find(szRandomSound);
							sound->VolumeSet(0.2f);
							sound->Play();
							float currentHp = obj->m_HealthPoint - m_pCharacter->m_Damage;
							if (currentHp <= 0)
								currentHp = 0;
							obj->m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage(currentHp / obj->m_kHealthPointMax, 1.0f));
							obj->m_pInterDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_pCharacter->m_Damage, obj->m_pInterDamage, 0.5f, 10.0f));
						}
						Damage(_blackboard, obj, m_pCharacter->m_Damage);
					}
				}
			}
		}
	}
	StateTransferPriority PlayerSkillRotate::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	std::vector<std::string> PlayerSkillRotate::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerIdle };
	}
	PlayerSkillPierceState::PlayerSkillPierceState() : PlayerStateCommonMethodInterface(0)
	{
	}
	void PlayerSkillPierceState::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsTransfer();

		if (!(0 < m_pCharacter->GetRemainSkillCoolTime(kPlayerPierce)))
		{
			if (Player::GetInstance().GetUltimateSkillStack() == 0)
			{
				ReserveNextTransferName(kPlayerPierce1);
				SetTransfer();
			}
			if (Player::GetInstance().GetUltimateSkillStack() == 1)
			{
				ReserveNextTransferName(kPlayerPierce2);
				SetTransfer();
			}
			if (Player::GetInstance().GetUltimateSkillStack() == 2)
			{
				ReserveNextTransferName(kPlayerPierce3);
				SetTransfer();
			}
		}

		if (IsTransfer())
		{
		}
	}
	void PlayerSkillPierceState::Action()
	{
	}
	StateTransferPriority PlayerSkillPierceState::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	std::vector<std::string> PlayerSkillPierceState::GetLinkedList()
	{
		return { kPlayerDead, kPlayerDash, kPlayerIdle, kPlayerPierce1, kPlayerPierce2, kPlayerPierce3 };
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
			Player::GetInstance().m_pInterSkillDash->m_pWorkList.push_back(new InterfaceFadeClockwise(Player::GetInstance().GetSkillCoolTime(kPlayerDash)));
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
		player->MoveChar(dir, world, true);
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
		{
			//pTrail->m_VertexList[i].c.w = 0.0f;
			//pTrail->m_VertexCatmullRomList[i].p = TVector3(0, 0, 0);
		}
	}
	StateTransferPriority PlayerDeadState::GetPriority()
	{
		return PlayerDeadTypePriority;
	}
	std::vector<std::string> PlayerDeadState::GetLinkedList()
	{
		return std::vector<std::string>();
	}
	PlayerSkillPierceState1::PlayerSkillPierceState1(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerSkillPierceState1::StateDecision()
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
			{
				//pTrail->m_VertexList[i].c.w = 0.0f;
				//pTrail->m_VertexCatmullRomList[i].p = TVector3(0, 0, 0);
			}

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
	void PlayerSkillPierceState1::Action()
	{
		if (!_blackboard->Initialized)
		{
			Player::GetInstance().ActiveSkill(kPlayerPierce);
			Player::GetInstance().m_AttackBoxLocalMatrix = TMatrix(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 100, -200, 1
			);

			Player::GetInstance().m_AttackBox.fExtent[0] = 0.5f;
			Player::GetInstance().m_AttackBox.fExtent[1] = 0.5f;
			Player::GetInstance().m_AttackBox.fExtent[2] = 2.0f;
			Player::GetInstance().m_pInterSkillQ->m_pWorkList.push_back(new InterfaceFadeClockwise(Player::GetInstance().GetSkillCoolTime(kPlayerPierce)));
		}

		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			std::wstring szRandomSound;
			switch ((int)randf(g_fGameTimer) % 6)
			{
			case 0:
				szRandomSound = L"yasuo_sound_attack1.mp3";
				break;
			case 1:
				szRandomSound = L"yasuo_sound_attack2.mp3";
				break;
			case 2:
				szRandomSound = L"yasuo_sound_attack3.mp3";
				break;
			case 3:
				szRandomSound = L"yasuo_sound_don.mp3";
				break;
			case 4:
				szRandomSound = L"yasuo_sound_hasegi1.mp3";
				break;
			case 5:
				szRandomSound = L"yasuo_sound_sul.mp3";
				break;
			}
			auto sound = I_Sound.Find(szRandomSound);
			sound->VolumeSet(0.2f);
			sound->Play();
			Player::GetInstance().m_pTrail->ResetTrail(&Player::GetInstance().m_matWorld);
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
		float startTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		float endTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.4f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > startTime &&
			m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < endTime
			)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						if (_blackboard->DamagedCharacters.find(obj) == _blackboard->DamagedCharacters.end())
						{
							std::wstring szRandomSound;
							switch ((int)randf(g_fGameTimer) % 2)
							{
							case 0:
								szRandomSound = L"sound_dmg_player1.mp3";
								break;
							case 1:
								szRandomSound = L"sound_dmg_player2.mp3";
								break;
							}
							auto sound = I_Sound.Find(szRandomSound);
							sound->VolumeSet(0.2f);
							sound->Play();
							float currentHp = obj->m_HealthPoint - m_pCharacter->m_Damage;
							if (currentHp <= 0)
								currentHp = 0;
							obj->m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage(currentHp / obj->m_kHealthPointMax, 1.0f));
							obj->m_pInterDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_pCharacter->m_Damage, obj->m_pInterDamage, 0.5f, 10.0f));
						}
						Damage(_blackboard, obj, m_pCharacter->m_Damage);
						Player::GetInstance().UltimateSkillStacking(_blackboard->StateTImeStamp);
					}
				}
			}
		}
	}
	StateTransferPriority PlayerSkillPierceState1::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	std::vector<std::string> PlayerSkillPierceState1::GetLinkedList()
	{
		return { kPlayerDead, kPlayerIdle, kPlayerDash };
	}
	PlayerSkillPierceState2::PlayerSkillPierceState2(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerSkillPierceState2::StateDecision()
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
			{
				//pTrail->m_VertexList[i].c.w = 0.0f;
				//pTrail->m_VertexCatmullRomList[i].p = TVector3(0, 0, 0);
			}

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
	void PlayerSkillPierceState2::Action()
	{
		if (!_blackboard->Initialized)
		{
			Player::GetInstance().ActiveSkill(kPlayerPierce);
			Player::GetInstance().m_AttackBoxLocalMatrix = TMatrix(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 100, -200, 1
			);

			Player::GetInstance().m_AttackBox.fExtent[0] = 0.5f;
			Player::GetInstance().m_AttackBox.fExtent[1] = 0.5f;
			Player::GetInstance().m_AttackBox.fExtent[2] = 2.0f;
			Player::GetInstance().m_pInterSkillQ->m_pWorkList.push_back(new InterfaceFadeClockwise(Player::GetInstance().GetSkillCoolTime(kPlayerPierce)));
		}

		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			std::wstring szRandomSound;
			switch ((int)randf(g_fGameTimer) % 6)
			{
			case 0:
				szRandomSound = L"yasuo_sound_attack1.mp3";
				break;
			case 1:
				szRandomSound = L"yasuo_sound_attack2.mp3";
				break;
			case 2:
				szRandomSound = L"yasuo_sound_attack3.mp3";
				break;
			case 3:
				szRandomSound = L"yasuo_sound_don.mp3";
				break;
			case 4:
				szRandomSound = L"yasuo_sound_hasegi1.mp3";
				break;
			case 5:
				szRandomSound = L"yasuo_sound_sul.mp3";
				break;
			}
			auto sound = I_Sound.Find(szRandomSound);
			sound->VolumeSet(0.2f);
			sound->Play();
			Player::GetInstance().m_pTrail->ResetTrail(&Player::GetInstance().m_matWorld);
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
		float startTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.2f;
		float endTime = m_pCharacter->m_pModel->_currentAnimation->_endFrame * 0.4f;
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime > startTime &&
			m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < endTime
			)
		{
			if (I_Collision.ChkPlayerAttackToNpcList(&m_pCharacter->m_AttackBox))
			{
				auto list = I_Collision.GetHitCharacterList(&m_pCharacter->m_AttackBox);
				for (auto obj : list)
				{
					if (obj != m_pCharacter)
					{
						if (_blackboard->DamagedCharacters.find(obj) == _blackboard->DamagedCharacters.end())
						{
							std::wstring szRandomSound;
							switch ((int)randf(g_fGameTimer) % 2)
							{
							case 0:
								szRandomSound = L"sound_dmg_player1.mp3";
								break;
							case 1:
								szRandomSound = L"sound_dmg_player2.mp3";
								break;
							}
							auto sound = I_Sound.Find(szRandomSound);
							sound->VolumeSet(0.2f);
							sound->Play();
							float currentHp = obj->m_HealthPoint - m_pCharacter->m_Damage;
							if (currentHp <= 0)
								currentHp = 0;
							obj->m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage(currentHp / obj->m_kHealthPointMax, 1.0f));
							obj->m_pInterDamage->m_pWorkList.push_back(new InterfaceDamageFloating(m_pCharacter->m_Damage, obj->m_pInterDamage, 0.5f, 10.0f));
						}
						Damage(_blackboard, obj, m_pCharacter->m_Damage);
						Player::GetInstance().UltimateSkillStacking(_blackboard->StateTImeStamp);
					}
				}
			}
		}
	}
	StateTransferPriority PlayerSkillPierceState2::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	std::vector<std::string> PlayerSkillPierceState2::GetLinkedList()
	{
		return { kPlayerDead, kPlayerIdle, kPlayerDash };
	}
	PlayerSkillPierceState3::PlayerSkillPierceState3(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerSkillPierceState3::StateDecision()
	{
		Decision_IsDead();

		Decision_IsDash();

		Decision_IsTransfer();

		if (Player::GetInstance().IsUlitmateSkillCallable())
		{
			Decision_IsSkill(kPlayerUltimate, 'R', 0);
		}

		if (IsTransfer())
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
			// 그럴려면 키를 누른 순간에 타이머를 사용하거나
			// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
			{
				//pTrail->m_VertexList[i].c.w = 0.0f;
				//pTrail->m_VertexCatmullRomList[i].p = TVector3(0, 0, 0);
			}
		}
	}
	void PlayerSkillPierceState3::Action()
	{
		if (!_blackboard->Initialized)
		{
			Player::GetInstance().ActiveSkill(kPlayerPierce);
			Player::GetInstance().UltimateSkillStacking(_blackboard->StateTImeStamp);
			Player::GetInstance().ShotTornado(_blackboard->StateTImeStamp);
			Player::GetInstance().m_pInterSkillQ->m_pWorkList.push_back(new InterfaceFadeClockwise(Player::GetInstance().GetSkillCoolTime(kPlayerPierce)));
		}

		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 0.1f)
		{
			std::wstring szRandomSound;
			switch ((int)randf(g_fGameTimer) % 6)
			{
			case 0:
				szRandomSound = L"yasuo_sound_attack1.mp3";
				break;
			case 1:
				szRandomSound = L"yasuo_sound_attack2.mp3";
				break;
			case 2:
				szRandomSound = L"yasuo_sound_attack3.mp3";
				break;
			case 3:
				szRandomSound = L"yasuo_sound_don.mp3";
				break;
			case 4:
				szRandomSound = L"yasuo_sound_hasegi1.mp3";
				break;
			case 5:
				szRandomSound = L"yasuo_sound_sul.mp3";
				break;
			}
			auto sound = I_Sound.Find(szRandomSound);
			sound->VolumeSet(0.2f);
			sound->Play();
			Player::GetInstance().m_pTrail->ResetTrail(&Player::GetInstance().m_matWorld);
		}

		static float timer = 0.0f;
		timer += g_fSecondPerFrame;
		if (timer > 0.02f)
		{
			Player::GetInstance().m_pTrail->AddTrailPos(
				Player::GetInstance().GetCurSocketPos("WeaponLow"), Player::GetInstance().GetCurSocketPos("WeaponHigh"));
			timer = 0.0f;
		}
	}
	StateTransferPriority PlayerSkillPierceState3::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	std::vector<std::string> PlayerSkillPierceState3::GetLinkedList()
	{
		return { kPlayerDead, kPlayerIdle, kPlayerDash, kPlayerUltimate };
	}
	PlayerUltimateSkillState::PlayerUltimateSkillState(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerUltimateSkillState::StateDecision()
	{
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			XMVECTOR tmp;
			XMMATRIX world = XMLoadFloat4x4(&m_pCharacter->m_matWorld);
			m_pCharacter->MoveChar(tmp, world);

			((CameraTPS*)((Player*)m_pCharacter)->m_pMainCamera)->m_CharCamDist = 10;

			for (auto elem : _blackboard->DamagedCharacters)
			{
				elem->Damage(100);
				if (_blackboard->DamagedCharacters.find(elem) == _blackboard->DamagedCharacters.end())
				{
					float currentHp = elem->m_HealthPoint - m_pCharacter->m_Damage;
					if (currentHp <= 0)
						currentHp = 0;
					elem->m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage(currentHp / elem->m_kHealthPointMax, 1.0f));
					elem->m_pInterDamage->m_pWorkList.push_back(new InterfaceDamageFloating(100, elem->m_pInterDamage, 0.5f, 10.0f));
				}
			}

			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}
	}
	void PlayerUltimateSkillState::Action()
	{
		Player* player = static_cast<Player*>(m_pCharacter);
		if (!_blackboard->Initialized)
		{
			std::wstring szRandomSound = L"yasuo_sound_soriegedon.mp3";
			auto sound = I_Sound.Find(szRandomSound);
			sound->VolumeSet(0.2f);
			sound->Play();
			Player::GetInstance().ActiveSkill(kPlayerPierce);

			((CameraTPS*)player->m_pMainCamera)->m_CharCamDist = 30;

			auto targets = player->GetUltimateSkillTargetList();
			Player::GetInstance().m_vPos = targets[0]->GetPosition();
			Player::GetInstance().m_vPos.y = 0;

			Player::GetInstance().m_pInterSkillR->m_pWorkList.push_back(new InterfaceFadeClockwise(Player::GetInstance().GetSkillCoolTime(kPlayerUltimate)));

			auto list = player->GetUltimateSkillTargetList();
			for (auto elem : list)
			{
				_blackboard->DamagedCharacters.insert(elem);
			}
		}

		auto list = player->GetUltimateSkillTargetList();
		for (auto elem : list)
		{
			elem->SetPoundState(true);
			elem->m_vPos.y = 3;
		}
	}
	StateTransferPriority PlayerUltimateSkillState::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	std::vector<std::string> PlayerUltimateSkillState::GetLinkedList()
	{
		return { kPlayerIdle };
	}
	PlayerDrinkSkillState::PlayerDrinkSkillState(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerDrinkSkillState::StateDecision()
	{
		if (_blackboard->Damaged)
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}

		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}

		if (IsTransfer())
		{
			if (!_blackboard->Damaged)
			{
				m_pCharacter->m_HealthPoint = min(m_pCharacter->m_HealthPoint + 30, m_pCharacter->m_kHealthPointMax);
				m_pCharacter->m_pInterGageHP->m_pWorkList.push_back(new InterfaceSetGage((float)m_pCharacter->m_HealthPoint / m_pCharacter->m_kHealthPointMax, 1.0f));
			}
		}
	}
	void PlayerDrinkSkillState::Action()
	{
		if (!_blackboard->Initialized)
		{
			std::wstring szRandomSound = L"yasuo_sound_drink.mp3";
			auto sound = I_Sound.Find(szRandomSound);
			sound->VolumeSet(0.3f);
			sound->Play();
			Player::GetInstance().ActiveSkill(kPlayerDrink);
			_blackboard->HealthPoint = m_pCharacter->m_HealthPoint;
		}

		if (_blackboard->HealthPoint != m_pCharacter->m_HealthPoint)
		{
			_blackboard->Damaged = true;
		}
	}
	StateTransferPriority PlayerDrinkSkillState::GetPriority()
	{
		return PlayerSkillTypePriority;
	}
	std::vector<std::string> PlayerDrinkSkillState::GetLinkedList()
	{
		return { kPlayerIdle };
	}
	PlayerHoudgiStartState::PlayerHoudgiStartState(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerHoudgiStartState::StateDecision()
	{
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			if (Player::GetInstance().IsBegin())
			{
				ReserveNextTransferName(kPlayerHoudgiEnd);
				SetTransfer();
			}
			else
			{
				ReserveNextTransferName(kPlayerHoudgiLoop);
				SetTransfer();
			}
		}
	}
	void PlayerHoudgiStartState::Action()
	{
	}
	std::vector<std::string> PlayerHoudgiStartState::GetLinkedList()
	{
		return { kPlayerHoudgiLoop, kPlayerHoudgiEnd };
	}
	PlayerHoudgiEndState::PlayerHoudgiEndState(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerHoudgiEndState::StateDecision()
	{
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerIdle);
			SetTransfer();
		}
	}
	void PlayerHoudgiEndState::Action()
	{
	}
	std::vector<std::string> PlayerHoudgiEndState::GetLinkedList()
	{
		return { kPlayerIdle };
	}
	PlayerVictoryStartState::PlayerVictoryStartState(float transferRequireTime) : PlayerStateCommonMethodInterface(transferRequireTime)
	{
	}
	void PlayerVictoryStartState::StateDecision()
	{
		if (IsPassedRequiredTime(_blackboard->StateTImeStamp))
		{
			ReserveNextTransferName(kPlayerVictoryLoop);
			SetTransfer();
		}
	}
	void PlayerVictoryStartState::Action()
	{
		if (_blackboard->Initialized)
		{
			Player::GetInstance().ActiveSkill(kPlayerRotate);
		}
	}
	StateTransferPriority PlayerVictoryStartState::GetPriority()
	{
		return PlayerVictoryTypePriority;
	}
	std::vector<std::string> PlayerVictoryStartState::GetLinkedList()
	{
		return { kPlayerVictoryLoop };
	}
	void PlayerVictoryLoopState::StateDecision()
	{
	}
	void PlayerVictoryLoopState::Action()
	{
	}
	std::vector<std::string> PlayerVictoryLoopState::GetLinkedList()
	{
		return std::vector<std::string>();
	}
}
;
