#pragma once

#include "CharacterState.h"

namespace SSB
{
	const StateName kBossMobIdle = "Idle";
	const StateName kBossMobAngry = "Angry";
	const StateName kBossMobMove = "Move";
	const StateName kBossMobAttack1 = "Attack1";
	const StateName kBossMobAttack2 = "Attack2";
	const StateName kBossMobDashStart = "DashStart";
	const StateName kBossMobDash = "Dash";
	const StateName kBossMobDashEnd = "DashEnd";
	const StateName kBossMobSkill1 = "Skill1";
	const StateName kBossMobSpawn = "Spawn";
	const StateName kBossMobStun = "Stun";
	const StateName kBossMobDead = "Dead";

	const unsigned int BossDeadTypePriority = 0;
	const unsigned int BossSkillTypePriority = 1;
	const unsigned int BossAttackTypePriority = 2;

	class BossMobIdleState : public CharacterState
	{
	public:
		void StateDecision() override;
		void Action() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class BossMobAngryState : public CharacterState, public MinimumTransferCoolTimeRequireInterface
	{
	private:
		float _transferRequireTime;

	public:
		BossMobAngryState(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class BossMobMoveState : public CharacterState
	{
	public:
		void StateDecision() override;
		void Action() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class BossMobAttack1State : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		BossMobAttack1State(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class BossMobAttack2State : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		BossMobAttack2State(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class BossMobDashStartState : public CharacterState, public MinimumTransferCoolTimeRequireInterface
	{
	private:
		float _transferRequireTime;

	public:
		BossMobDashStartState(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class BossMobDashState : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		BossMobDashState(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class BossMobDashEndState : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		BossMobDashEndState(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class BossMobSkill1State : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		BossMobSkill1State(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class BossMobSpawnState : public CharacterState, public MinimumTransferCoolTimeRequireInterface
	{
	private:
		float _transferRequireTime;

	public:
		BossMobSpawnState(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	//class BossMobStunState : public CharacterState
	//{
	//public:
	//	bool IsTransfer() override;
	//};

	class BossMobDeadState : public CharacterState
	{
	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};
}
