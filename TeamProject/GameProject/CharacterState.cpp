#include "CharacterState.h"
#include "Player.h"

namespace SSB
{
	void CharacterState::ReserveNextTransferName(StateName transferStateName)
	{
		CharacterState* compareState = _linkedState.find(transferStateName)->second;
		if (_reservedState != nullptr)
		{
			StateTransferPriority currentPriority = _reservedState->GetPriority();
			StateTransferPriority comparePriority = compareState->GetPriority();
			if (comparePriority < currentPriority)
			{
				_reservedState = compareState;
			}
		}
		else
		{
			_reservedState = compareState;
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
		return 1000;
	}
	void CharacterState::Run()
	{
		m_pCharacter->SetCurrentAnimation(GetStateAnimationName());
	}
}
