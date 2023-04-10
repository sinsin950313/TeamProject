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
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerMoveState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerAttackState1 : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerAttackState2 : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerAttackState3 : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerAttackState4 : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerSkillState1 : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerSkillState2 : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerSkillState3 : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerSkillState4 : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerSkillState5 : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerDashState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class PlayerDeadState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};
}
