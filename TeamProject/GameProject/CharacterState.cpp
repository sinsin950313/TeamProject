#include "CharacterState.h"

namespace SSB
{
	void CharacterState::SetNextTransferName(StateName transferStateName)
	{
		m_TransferStateName = transferStateName;
	}
	void CharacterState::Initialize_SetStateAnimation(AnimationName name)
	{
		m_StateAnimationName = name;
	}
	void CharacterState::SetCharacter(Character* character)
	{
		m_pCharacter = character;
	}
	AnimationName CharacterState::GetStateAnimationName()
	{
		return m_StateAnimationName;
	}
	StateName CharacterState::GetNextTransferStateName()
	{
		return m_TransferStateName;
	}
}
