#pragma once

#include "CharacterState.h"

namespace SSB
{
	const StateName kPlayerIdle = "Idle";
	const StateName kPlayerMove = "Move";
	const StateName kPlayerAttack1 = "Attack1";
	const StateName kPlayerAttack2 = "Attack2";
	const StateName kPlayerAttack3 = "Attack3";
	const StateName kPlayerDead = "Dead";

	class PlayerIdleState : public CharacterState
	{
	public:
		bool IsTransfer() override;
	};

	class PlayerMoveState : public CharacterState
	{
	public:
		bool IsTransfer() override;
		void Run() override;
	};

	class PlayerAttackState1 : public CharacterState
	{
	public:
		bool IsTransfer() override;
		void Run() override;
	};

	class PlayerAttackState2 : public CharacterState
	{
	public:
		bool IsTransfer() override;
		void Run() override;
	};

	class PlayerAttackState3 : public CharacterState
	{
	public:
		bool IsTransfer() override;
		void Run() override;
	};

	class PlayerDeadState : public CharacterState
	{
	public:
		bool IsTransfer() override;
	};
}
