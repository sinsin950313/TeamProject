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

	class BossMobIdleState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class BossMobAngryState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class BossMobMoveState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class BossMobAttack1State : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class BossMobAttack2State : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class BossMobDashStartState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class BossMobDashState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class BossMobDashEndState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class BossMobSkill1State : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	class BossMobSpawnState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};

	//class BossMobStunState : public CharacterState
	//{
	//public:
	//	bool IsTransfer() override;
	//};

	class BossMobDeadState : public CharacterState
	{
	public:
		void Run() override;
		StateTransferPriority GetPriority() override;
	};
}
