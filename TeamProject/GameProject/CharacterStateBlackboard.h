#pragma once

#include <map>
#include "Character.h"
#include "CharacterState.h"
#include "Sound.h"

namespace SSB
{
	struct Blackboard
	{
		float StateTImeStamp = 0;
		float BeforeTime = 0;
		float StateElapseTime = 0;
		Sound* CurrentSound;
		std::set<Character*> DamagedCharacters;
		CharacterState* ReservedState;
	};

	class CharacterStateBlackboardManager
	{
	private:
		std::map<Character*, Blackboard> _data;

	public:
		void RegisterBlackboard(Blackboard blackboard);
		Blackboard GetBlackBoard(Character* character);
		void SetBlackboard(Character* character, Blackboard blackboard);
	};
}
