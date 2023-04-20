#pragma once

#include "CharacterState.h"

namespace SSB
{
	const StateName kEnemyNPCMobIdle = "Idle";
	const StateName kEnemyNPCMobMove = "Move";
	const StateName kEnemyNPCMobAttack1 = "Attack1";
	const StateName kEnemyNPCMobAttack2 = "Attack2";
	const StateName kEnemyNPCMobDead = "Dead";
	const StateName kEnemyNPCMobAirborne = "Airborne";
	const StateName kEnemyNPCMobPound = "Pound";

	const unsigned int EnemyNPCMobDeadTypePriority = 0;
	const unsigned int EnemyNPCMobAirborneTypePriority = 1;
	const unsigned int EnemyNPCMobAttackTypePriority = 2;
	const unsigned int EnemyNPCMobMoveTypePriority = 3;

	class EnemyNPCMobIdleState : public CharacterState
	{
	public:
		void StateDecision() override;
		void Action() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class EnemyNPCMobAirBorneState : public CharacterState
	{
	private:
		// If change this value, consider PlayerStateService::PlayerUltimateSkillState::Action::elem->m_vPos.y
		float _airborneHeight = 3;

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class EnemyNPCMobPoundState : public CharacterState, public MinimumTransferCoolTimeRequireInterface
	{
	private:
		float _transferRequireTime;

	public:
		EnemyNPCMobPoundState(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		float GetTransferRequireTime() override;
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
