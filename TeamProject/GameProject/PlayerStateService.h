#pragma once

#include "CharacterState.h"

namespace SSB
{
	class PlayerIdleState : public CharacterState
	{
	public:
		bool IsTransfer() override;
		void Run() override;
	};

	class PlayerMoveState : public CharacterState
	{
	public:
		bool IsTransfer() override;
		void Run() override;
	};

	class PlayerAttackState : public CharacterState
	{
	public:
		bool IsTransfer() override;
		void Run() override;
	};
}
