#include "CharacterStateBlackboard.h"

namespace SSB
{
	CharacterStateBlackboardManager::~CharacterStateBlackboardManager()
	{
		for (auto iter : _data)
		{
			delete iter.second;
		}
		_data.clear();
	}
	void CharacterStateBlackboardManager::RegisterBlackboard(Character* character)
	{
		Blackboard* tmp = new Blackboard;
		tmp->StateTImeStamp = g_fGameTimer;
		_data.insert(std::make_pair(character, tmp));
	}
	Blackboard* CharacterStateBlackboardManager::GetBlackBoard(Character* character)
	{
		return _data.find(character)->second;
	}
}
