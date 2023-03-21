#include "PlayerStateService.h"
#include "Input.h"
#include "Player.h"

namespace SSB
{
	bool PlayerIdleState::IsTransfer()
	{
		bool transfer = false;

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

		if (m_pCharacter->IsDead())
		{
			transfer = true;
			SetNextTransferName(kPlayerDead);
		}

		return transfer;
	}
	void PlayerIdleState::Run()
	{
        m_pCharacter->m_pModel->SetCurrentAnimation("Idle");
		OutputDebugString(L"Idle\n");
	}
	bool PlayerMoveState::IsTransfer()
	{
		bool transfer = false;

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

		if (m_pCharacter->IsDead())
		{
			transfer = true;
			SetNextTransferName(kPlayerDead);
		}

		return transfer;
	}
	void PlayerMoveState::Run()
	{
        m_pCharacter->m_pModel->SetCurrentAnimation("Move");

		Player* player = static_cast<Player*>(m_pCharacter);

		bool moveChar = false;
		XMVECTOR desiredCharDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		if (I_Input.GetKey('A') == KEY_HOLD)
		{
			desiredCharDir += (player->m_pMainCamera->m_vRight);
			moveChar = true;
		}
		if (I_Input.GetKey('D') == KEY_HOLD)
		{
			desiredCharDir += -(player->m_pMainCamera->m_vRight);
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

		if (I_Input.GetKey(VK_LBUTTON) != KEY_PUSH && 
			I_Input.GetKey(VK_LBUTTON) != KEY_HOLD)
		{
			transfer = true;
			SetNextTransferName(kPlayerIdle);
		}

		if (m_pCharacter->IsDead())
		{
			transfer = true;
			SetNextTransferName(kPlayerDead);
		}

		return transfer;
	}
	void PlayerAttackState::Run()
	{
        m_pCharacter->m_pModel->SetCurrentAnimation("Attack1");

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
		OutputDebugString(L"Attack\n");
	}
}
