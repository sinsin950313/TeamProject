#pragma once

#include "CharacterState.h"

namespace SSB
{
	const StateName kPlayerIdle = "Idle";
	const StateName kPlayerMove = "Move";
	const StateName kPlayerAttack = "Attack";
	const StateName kPlayerDead = "Dead";

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

	//class PlayerDeadState : public CharacterState
	//{
	//public:
	//	bool IsTransfer() override;
	//	void Run() override;
	//};
}
