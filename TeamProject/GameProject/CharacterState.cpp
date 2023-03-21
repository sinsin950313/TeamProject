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
	void CharacterState::Initialize_SetCoolTime(float cooltime)
	{
		m_Cooltime = cooltime;
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
	bool CharacterState::IsPassedRequireCoolTime(float elapseTime)
	{
		return m_Cooltime <= elapseTime;
	}
	void CharacterState::Run()
	{
        m_pCharacter->m_pModel->SetCurrentAnimation(GetStateAnimationName());
	}
}
