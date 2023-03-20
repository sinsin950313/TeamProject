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
	std::map<T_BOX*, Character*> test;
	std::map<T_BOX*, Character*> m_StaticObjectList;

	// void* -> NpcAttack : Attack 형식의 클래스로 변경예정..?
	std::map<T_BOX*, void*> m_NpcAttackList;

public:
	bool	Frame();

	void	AddBox(T_BOX* box, Character* pChar);
	void	AddStaticObjectBox(T_BOX* box, Character* pChar);
	void	DeleteBox(T_BOX* box);

private:
	CollisionMgr();
public:
	~CollisionMgr();
};

#define I_Collision CollisionMgr::GetInstance()

