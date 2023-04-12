#pragma once

#include "CharacterState.h"

namespace SSB
{
	const StateName kEnemyNPCMobIdle = "Idle";
	const StateName kEnemyNPCMobMove = "Move";
	const StateName kEnemyNPCMobAttack1 = "Attack1";
	const StateName kEnemyNPCMobAttack2 = "Attack2";
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

	class EnemyNPCMobAttack1State : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		EnemyNPCMobAttack1State(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class EnemyNPCMobAttack2State : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		EnemyNPCMobAttack2State(float transferRequireTime);

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
