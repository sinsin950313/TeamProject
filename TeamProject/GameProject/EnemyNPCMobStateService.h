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
		void Run() override;
	};

	class EnemyNPCMobMoveState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class EnemyNPCMobAttackState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class EnemyNPCMobDeadState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};
}
