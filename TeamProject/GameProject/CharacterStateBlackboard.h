#pragma once

#include <map>
#include "Character.h"
#include "SoundEffect.h"

namespace SSB
{
	struct Blackboard
	{
		float StateTImeStamp = 0;
		bool Initialized = false;
		SoundEffect CurrentSound;
		std::set<Character*> DamagedCharacters;
		float HealthPoint;
		bool Damaged;
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
