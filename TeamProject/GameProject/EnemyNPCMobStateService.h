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
		void StateDecision() override;
		void Action() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class EnemyNPCMobMoveState : public CharacterState
	{
	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class EnemyNPCMobAttackState : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		EnemyNPCMobAttackState(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class EnemyNPCMobDeadState : public CharacterState
	{
	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};
}
