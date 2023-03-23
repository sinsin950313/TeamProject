#pragma once
#include "Character.h"

class CollisionMgr : public Singleton<CollisionMgr>
{
private:
	friend class Singleton<CollisionMgr>;
	/*std::map<Character*, T_BOX*>	m_List;*/
	/*
	StaticMeshList

	NpcList
	Player < Singleton

	PlayerAttackList
	NpcAttackList
	*/
	std::map<T_BOX*, Character*> m_StaticObjectList;
	std::map<T_BOX*, Character*>	m_NpcList;

	// Character* -> NpcAttack : Attack 형식의 클래스로 변경예정..?
	std::map<T_BOX*, Character*> m_NpcAttackList;

public:
	bool	ChkPlayerAttackToNpcList(T_BOX* box);
	std::vector<Character*> GetHitCharacterList(T_BOX* attackBox);
	bool IsCollide(T_BOX* box);

public:
	void	AddStaticObjectBox(T_BOX* box, Character* pChar);
	void	AddNpcBox(T_BOX* box, Character* pChar);
	void	DeleteBox(T_BOX* box);

private:
	CollisionMgr();
public:
	~CollisionMgr();
};

#define I_Collision CollisionMgr::GetInstance()

