#include "CharacterState.h"
#include "Player.h"

namespace SSB
{
	void CharacterState::ReserveNextTransferName(StateName transferStateName)
	{
		CharacterState* transferCompareState = _linkedState.find(transferStateName)->second;
		if (_reservedState != nullptr)
		{
			StateTransferPriority currentPriority = _reservedState->GetPriority();
			StateTransferPriority comparePriority = transferCompareState->GetPriority();
			if (comparePriority < currentPriority)
			{
				_reservedState = transferCompareState;
			}
		}
		else
		{
			_reservedState = transferCompareState;
		}

		_isReservedTransfer = true;
	}
	bool CharacterState::IsReservingNextState()
	{
		return _isReservedTransfer;
	}
	void CharacterState::Initialize_SetStateAnimation(AnimationName name)
	{
		m_StateAnimationName = name;
	}
	void CharacterState::Initialize_SetEffectSound(Sound* sound, bool loop)
	{
		_sound = sound;
		_loop = loop;
	}
	void CharacterState::Initialize_LinkState(StateName stateName, CharacterState* state)
	{
		_linkedState.insert(std::make_pair(stateName, state));
	}
	void CharacterState::SetData(Character* character, Blackboard* blackboard)
	{
		m_pCharacter = character;
		_reservedState = nullptr;
		_isReservedTransfer = false;
		_isTransfer = false;
		_blackboard = blackboard;
	}
	AnimationName CharacterState::GetStateAnimationName()
	{
		return m_StateAnimationName;
	}
	CharacterState* CharacterState::GetReservedState()
	{
		return _reservedState;
	}
	Sound* CharacterState::GetSound()
	{
		return _sound;
	}
	bool CharacterState::IsSoundLoop()
	{
		return _loop;
	}
	void CharacterState::SetTransfer()
	{
		_isTransfer = true;
	}
	bool CharacterState::IsTransfer()
	{
		return _isTransfer;
	}
	StateTransferPriority CharacterState::GetPriority()
	{
		return -1;
	}
	void CharacterState::Run()
	{
		m_pCharacter->SetCurrentAnimation(GetStateAnimationName());

		Action();
		StateDecision();
	}
	bool MinimumTransferCoolTimeRequireInterface::IsPassedRequiredTime(float timeStamp)
	{
		return g_fGameTimer - timeStamp >= GetTransferRequireTime();
	}
	void DamageTypeStateInterface::Damage(Blackboard* blackboard, Character* target, float damage)
	{
		if (blackboard->DamagedCharacters.find(target) == blackboard->DamagedCharacters.end())
		{
			target->Damage(damage);
			blackboard->DamagedCharacters.insert(&Player::GetInstance());
		}
	}
}
