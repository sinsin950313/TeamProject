#pragma once

#include "Common.h"
#include <map>
#include "Animation.h"
#include "Character.h"
#include "Sound.h"
#include <map>
#include "CharacterStateBlackboard.h"

namespace SSB
{
	typedef std::string StateName;
	typedef unsigned int StateTransferPriority;

	class CharacterState
	{
	private:
		AnimationName m_StateAnimationName;
		std::map<StateName, CharacterState*> _linkedState;

	public:
		void Initialize_SetStateAnimation(AnimationName name);
		void Initialize_SetEffectSound(Sound* sound, bool loop = false);
		void Initialize_LinkState(StateName stateName, CharacterState* state);

	private:
		Sound* _sound = nullptr;
		bool _loop = false;

	public:
		Sound* GetSound();
		bool IsSoundLoop();

	protected:
		Character* m_pCharacter = nullptr;
		Blackboard* _blackboard;

	public:
		void SetData(Character* character, Blackboard* blackboard);
		AnimationName GetStateAnimationName();

	private:
		CharacterState* _reservedState;
		bool _isReservedTransfer = false;

	protected:
		void ReserveNextTransferName(StateName transferStateName);

	public:
		bool IsReservingNextState();
		CharacterState* GetReservedState();

	private:
		bool _isTransfer;

	protected:
		void SetTransfer();

	public:
		bool IsTransfer();
		void Run();

	public:
		virtual StateTransferPriority GetPriority();

	protected:
		virtual void StateDecision() = 0;
		virtual void Action() = 0;

	public:
		virtual std::vector<std::string> GetLinkedList() = 0;
	};

	class MinimumTransferCoolTimeRequireInterface
	{
	protected:
		virtual float GetTransferRequireTime() = 0;

	public:
		bool IsPassedRequiredTime(float timeStamp);
	};

	class DamageTypeStateInterface
	{
	public:
		void Damage(Blackboard* blackboard, Character* target, float damage);
	};
}
