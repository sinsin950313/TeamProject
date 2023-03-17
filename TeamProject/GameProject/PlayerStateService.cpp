#include "PlayerStateService.h"
#include "Input.h"
#include "PlayerTransferStateName.h"
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

		return transfer;
	}
	void PlayerIdleState::Run()
	{
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

		return transfer;
	}
	void PlayerMoveState::Run()
	{
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

		return transfer;
	}
	void PlayerAttackState::Run()
	{
		OutputDebugString(L"Attack\n");
	}
}
