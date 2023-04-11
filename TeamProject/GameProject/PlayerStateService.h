#pragma once

#include "CharacterState.h"

namespace SSB
{
	const StateName kPlayerIdle = "Idle";
	const StateName kPlayerMove = "Move";
	const StateName kPlayerAttack1 = "Attack1";
	const StateName kPlayerAttack2 = "Attack2";
	const StateName kPlayerAttack3 = "Attack3";
	const StateName kPlayerAttack4 = "Attack4";
	const StateName kPlayerSkill1 = "Skill1";
	const StateName kPlayerSkill2 = "Skill2";
	const StateName kPlayerSkill3 = "Skill3";
	const StateName kPlayerSkill4 = "Skill4";
	const StateName kPlayerSkill5 = "Skill5";
	const StateName kPlayerDash = "Dash";
	const StateName kPlayerDead = "Dead";

	const unsigned int PlayerDeadTypePriority = 0;
	const unsigned int PlayerDashTypePriority = 1;
	const unsigned int PlayerSkillTypePriority = 2;
	const unsigned int PlayerAttackTypePriority = 3;

	class PlayerIdleState : public CharacterState
	{
	public:
		void StateDecision() override;
		void Action() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerMoveState : public CharacterState
	{
	public:
		void StateDecision() override;
		void Action() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerAttackState1 : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerAttackState1(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action();
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerAttackState2 : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerAttackState2(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action();
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerAttackState3 : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerAttackState3(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action();
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerAttackState4 : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerAttackState4(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action();
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerSkillState1 : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerSkillState1(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerSkillState2 : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerSkillState2(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerSkillState3 : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerSkillState3(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerSkillState4 : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerSkillState4(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerSkillState5 : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerSkillState5(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerDashState : public CharacterState, public MinimumTransferCoolTimeRequireInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerDashState(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		float GetTransferRequireTime() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerDeadState : public CharacterState
	{
	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};
}
