#include "PlayerStateService.h"
#include "Input.h"
#include "Player.h"

namespace SSB
{
	bool PlayerIdleState::IsTransfer()
	{
		bool transfer = false;

		if (I_Input.GetKey('A') == KEY_HOLD)
		{
			transfer = true;
			SetNextTransferName(kPlayerMove);
		}
		if (I_Input.GetKey('D') == KEY_HOLD)
		{
			transfer = true;
			SetNextTransferName(kPlayerMove);
		}
		if (I_Input.GetKey('W') == KEY_HOLD)
		{
			transfer = true;
			SetNextTransferName(kPlayerMove);
		}
		if (I_Input.GetKey('S') == KEY_HOLD)
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

		float speed = 10.0f * g_fSecondPerFrame;
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

		OutputDebugString(L"Attack\n");
	}
}
