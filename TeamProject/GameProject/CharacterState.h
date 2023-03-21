#pragma once

#include "Common.h"
#include <map>
#include "Animation.h"
#include "Character.h"

namespace SSB
{
	typedef std::string StateName;

	class CharacterState
	{
	private:
		StateName m_TransferStateName;
		AnimationName m_StateAnimationName;
		float m_Cooltime = 0;

	protected:
		Character* m_pCharacter = nullptr;

	protected:
		void SetNextTransferName(StateName transferStateName);

	public:
		void Initialize_SetStateAnimation(AnimationName name);
		void Initialize_SetCoolTime(float cooltime);
		void SetCharacter(Character* character);
		AnimationName GetStateAnimationName();
		StateName GetNextTransferStateName();
		bool IsPassedRequireCoolTime(float elapseTime);

	public:
		virtual bool IsTransfer() = 0;
		virtual void Run();
	};
}
