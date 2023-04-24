#pragma once
#include "Character.h"

struct CollisionData
{
	TVector3 CollisionNormal;
	float CollisionDepth;
};

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
	std::vector<T_BOX> m_MapCollisionList;
	std::map<std::wstring, T_BOX> m_MapTriggerList;
	std::map<T_BOX*, Character*> m_StaticObjectList;
	std::map<T_BOX*, Character*> m_NpcList;

public:
	bool	ChkPlayerAttackToNpcList(T_BOX* box);
	bool	ChkCharacterToStaticObjList(T_BOX* box);
	std::vector<Character*> GetHitCharacterList(T_BOX* attackBox);
	bool IsCollide(T_BOX* box);
	bool IsCollideTrigger(T_BOX* box);

public:
	void	AddStaticObjectBox(T_BOX* box, Character* pChar);
	void	AddNpcBox(T_BOX* box, Character* pChar);
	void	DeleteBox(T_BOX* box);

public:
	void	AddMapCollisionBox(T_BOX tBox);
	void	AddMapTriggerBox(std::wstring szName, T_BOX tBox);
	void	DeleteTriggerBox(std::wstring szName);
	std::vector<T_BOX>& GetMapCollisionList();
	std::map<std::wstring, T_BOX>& GetMapTriggerList();

private:
	bool IsPenetratable(std::vector<TVector3> planeVertice, T_PLANE plane, TVector3 vertex);
	bool IsPenetrate(T_PLANE plane, TVector3 start, TVector3 end);

public:
	std::vector<T_BOX> GetCollideBoxList(T_BOX* collider, bool ghost = false);
	std::vector<CollisionData> GetCollideData(T_BOX source, T_BOX dest);

private:
	CollisionMgr();
public:
	~CollisionMgr();

public:
	void NPCIsDead(T_BOX* box);
};

#define I_Collision CollisionMgr::GetInstance()

