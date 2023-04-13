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
	const unsigned int PlayerMoveTypePriority = 4;

	class PlayerStateCommonMethodInterface : public CharacterState, public MinimumTransferCoolTimeRequireInterface, public DamageTypeStateInterface
	{
	private:
		float _transferRequireTime;

	public:
		PlayerStateCommonMethodInterface(float transferRequireTime = 0);

	protected:
		void Decision_IsDead();
		void Decision_IsDash();
		void Decision_IsSkill(SSB::StateName skillStateName, char key, float minimumRequiredTime);
		void Decision_IsAttack(SSB::StateName nextAttackStateName, float minimumRequiredTime);
		void Decision_IsMove();
		void Decision_IsTransfer();

	protected:
		float GetTransferRequireTime() override;

	protected:
		//void Action_();
	};

	class PlayerIdleState : public PlayerStateCommonMethodInterface
	{
	public:
		void StateDecision() override;
		void Action() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerMoveState : public PlayerStateCommonMethodInterface
	{
	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerAttackState1 : public PlayerStateCommonMethodInterface
	{
	public:
		PlayerAttackState1(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action();
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerAttackState2 : public PlayerStateCommonMethodInterface
	{
	public:
		PlayerAttackState2(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action();
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerAttackState3 : public PlayerStateCommonMethodInterface
	{
	public:
		PlayerAttackState3(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action();
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerAttackState4 : public PlayerStateCommonMethodInterface
	{
	public:
		PlayerAttackState4(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action();
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerSkillState1 : public PlayerStateCommonMethodInterface
	{
	public:
		PlayerSkillState1(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerSkillState2 : public PlayerStateCommonMethodInterface
	{
	public:
		PlayerSkillState2(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
		std::vector<std::string> GetLinkedList() override;
	};

	class PlayerDashState : public PlayerStateCommonMethodInterface
	{
	public:
		PlayerDashState(float transferRequireTime);

	public:
		void StateDecision() override;
		void Action() override;
		StateTransferPriority GetPriority() override;
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
