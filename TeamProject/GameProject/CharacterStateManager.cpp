#include "CharacterStateManager.h"

namespace SSB
{
	void CharacterStateManager::Initialize_RegisterState(StateName name, CharacterState* state)
	{
		m_StateMap.insert(std::make_pair(name, state));
	}
	void CharacterStateManager::RegisterCharacter(Character* character, StateName initialStateName)
	{
		m_CharacterStateMap.insert(std::make_pair(character, m_StateMap.find(initialStateName)->second));
	}
	bool CharacterStateManager::Init()
	{
		for (auto iter : m_StateMap)
		{
			delete iter.second;
		}
		m_StateMap.clear();
		m_CharacterStateMap.clear();

		return true;
	}
	bool CharacterStateManager::Frame()
	{
		for (auto iter : m_CharacterStateMap)
		{
			Character* character = iter.first;
			CharacterState* state = iter.second;

			state->SetCharacter(character);
			state->Run();

			if (state->IsTransfer())
			{
				if (character->_currentSound != nullptr)
				{
					character->_currentSound->Stop();
				}

				CharacterState* newState = m_StateMap.find(state->GetNextTransferStateName())->second;
				m_CharacterStateMap[character] = newState;
				character->ResetStateElapseTime();

				CharacterState* nextState = m_StateMap.find(state->GetNextTransferStateName())->second;
				character->_currentSound = nextState->GetSound();
				if (character->_currentSound != nullptr)
				{
					character->_currentSound->Play(nextState->IsSoundLoop());
				}
			}
			else
			{
				if (state->IsPassedRequireCoolTime(character->GetStateElapseTime()))
				{
					character->ResetStateElapseTime();
				}
			}
		}

		return true;
	}
	bool CharacterStateManager::Render()
	{
		return true;
	}
	bool CharacterStateManager::Release()
	{
		for (auto iter : m_StateMap)
		{
			delete iter.second;
		}
		m_StateMap.clear();
		m_CharacterStateMap.clear();

		return true;
	}
}
