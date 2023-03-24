#include "PlayerStateService.h"
#include "Input.h"
#include "Player.h"
#include "CollisionMgr.h"

namespace SSB
{
	bool PlayerIdleState::IsTransfer()
	{
		bool transfer = false;

		if (m_pCharacter->IsDead())
		{
			transfer = true;
			SetNextTransferName(kPlayerDead);
		}
		else
		{
			if (I_Input.GetKey('A') == KEY_HOLD ||
				I_Input.GetKey('D') == KEY_HOLD ||
				I_Input.GetKey('W') == KEY_HOLD ||
				I_Input.GetKey('S') == KEY_HOLD)
			{
				transfer = true;
				SetNextTransferName(kPlayerMove);
			}

			if (I_Input.GetKey(VK_LBUTTON) == KEY_PUSH)
			{
				transfer = true;
				SetNextTransferName(kPlayerAttack);
			}
		}

		return transfer;
	}
	bool PlayerMoveState::IsTransfer()
	{
		bool transfer = false;

		if (m_pCharacter->IsDead())
		{
			transfer = true;
			SetNextTransferName(kPlayerDead);
		}
		else
		{
			if (I_Input.GetKey(VK_LBUTTON) == KEY_PUSH)
			{
				transfer = true;
				SetNextTransferName(kPlayerAttack);
			}

			if (!transfer)
			{
				if (I_Input.GetKey('A') != KEY_HOLD &&
					I_Input.GetKey('D') != KEY_HOLD &&
					I_Input.GetKey('W') != KEY_HOLD &&
					I_Input.GetKey('S') != KEY_HOLD)
				{
					transfer = true;
					SetNextTransferName(kPlayerIdle);
				}
			}
		}

		return transfer;
	}
	void PlayerMoveState::Run()
	{
        CharacterState::Run();
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
	bool PlayerAttackState::IsTransfer()
	{
		bool transfer = false;

		// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
		// 키를 떼도 공격키를 눌렀으면 공격이 진행돼야함
		// 그럴려면 키를 누른 순간에 타이머를 사용하거나
		// 현재의 애니메이션이 끝났나 안끝났나를 체크해야함
		if (m_pCharacter->IsDead())
		{
			auto pTrail = Player::GetInstance().m_pTrail;
			for (int i = 0; i < pTrail->m_VertexList.size(); i++)
				pTrail->m_VertexList[i].c.w = 0.0f;

			transfer = true;
			SetNextTransferName(kPlayerDead);
		}
		else
		{
			// 어택 타이머 > 0, isAttack != true, 애니메이션의 끝 알림등의 조건이 추가될 필요 있음
			if (IsPassedRequireCoolTime(m_pCharacter->GetStateElapseTime()))
			{
				if (I_Input.GetKey(VK_LBUTTON) != KEY_PUSH &&
					I_Input.GetKey(VK_LBUTTON) != KEY_HOLD)
				{
					transfer = true;
					SetNextTransferName(kPlayerIdle);
				}
			}
		}

		return transfer;
	}
	void PlayerAttackState::Run()
	{
		CharacterState::Run();

        //m_pCharacter->m_pModel->SetCurrentAnimation("Attack1");
		if (m_pCharacter->m_pModel->_currentAnimation->m_fAnimTime < 1.0f)
		{
			Player::GetInstance().m_pTrail->m_iPos = 0;
			int size = Player::GetInstance().m_pTrail->m_IndexList.size();
			int p = Player::GetInstance().m_pTrail->m_VertexList.size() - 1;
			//for (int i = 0; i < size; i++)
			//	Player::GetInstance().m_pTrail->m_IndexList[i] = p;
			//int a = 4;
		}

		static float timer = 0.0f;
		timer += g_fSecondPerFrame;
		if (timer > 0.05f)
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
						if (!m_pCharacter->IsAlreadyDamagedCurrentState(obj))
						{
							obj->Damage(m_pCharacter->m_Damage);
							m_pCharacter->DamagingCharacter(obj);
						}
					}
				}
			}
		}
	}
	bool PlayerDeadState::IsTransfer()
	{
		return false;
	}
}
