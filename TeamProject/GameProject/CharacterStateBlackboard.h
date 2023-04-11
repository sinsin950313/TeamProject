#pragma once

#include <map>
#include "Character.h"
#include "Sound.h"

namespace SSB
{
	struct Blackboard
	{
		float StateTImeStamp = 0;
		float BeforeTime = 0;
		Sound* CurrentSound;
		std::set<Character*> DamagedCharacters;
	};

	class CharacterStateBlackboardManager
	{
	private:
		std::map<Character*, Blackboard*> _data;

	public:
		~CharacterStateBlackboardManager();

	public:
		void RegisterBlackboard(Character* character);
		Blackboard* GetBlackBoard(Character* character);
	};
}
