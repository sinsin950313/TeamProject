#include "CharacterStateManager.h"
#include "Player.h"

namespace SSB
{
	void CharacterStateManager::RegisterReservedState(Blackboard* blackboard, Character* character, CharacterState* compareState)
	{
		CharacterState* currentReservedState = m_ReservedState.find(character)->second;
		if (currentReservedState != nullptr)
		{
			StateTransferPriority currentPriority = currentReservedState->GetPriority();
			StateTransferPriority comparePriority = compareState->GetPriority();
			if (currentPriority < comparePriority)
			{
				m_ReservedState.find(character)->second = compareState;
			}
		}
		else
		{
			m_ReservedState.find(character)->second = compareState;
		}
	}
	void CharacterStateManager::PrepareForTransfer(Blackboard* blackboard, Character* character)
	{
		m_ReservedState.find(character)->second = nullptr;

		if (blackboard->CurrentSound != nullptr)
		{
			blackboard->CurrentSound->Stop();
		}
		blackboard->DamagedCharacters.clear();
		blackboard->BeforeTime = ;
		blackboard->StateElapseTime = ;
		blackboard->StateTImeStamp = ;
	}
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
			Blackboard* blackboard = _blackboardManager.GetBlackBoard(character);
			CharacterState* state = iter.second;

			state->SetData(character, blackboard);
			state->Run();

			if (state->IsReservingNextState())
			{
				RegisterReservedState(blackboard, character, state);
			}

			if (state->IsTransfer())
			{
				PrepareForTransfer(blackboard, character);

				auto reservedState = m_ReservedState.find(character)->second;
				m_CharacterStateMap[character] = reservedState;
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
