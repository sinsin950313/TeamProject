#pragma once

#include "CharacterState.h"

namespace SSB
{
	const StateName kEnemyNPCMobIdle = "Idle";
	const StateName kEnemyNPCMobMove = "Move";
	const StateName kEnemyNPCMobAttack = "Attack";
	const StateName kEnemyNPCMobDead = "Dead";

	class EnemyNPCMobIdleState : public CharacterState
	{
	public:
		bool IsTransfer() override;
	};

	class EnemyNPCMobMoveState : public CharacterState
	{
	public:
		bool IsTransfer() override;
		void Run() override;
	};

	class EnemyNPCMobAttackState : public CharacterState
	{
	public:
		bool IsTransfer() override;
		void Run() override;
	};

	class EnemyNPCMobDeadState : public CharacterState
	{
	public:
		bool IsTransfer() override;
	};
}
