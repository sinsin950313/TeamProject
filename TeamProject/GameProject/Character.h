#pragma once

#include "Model.h"
#include "TSelect.h"
#include <map>
#include "Sound.h"
#include "Interface.h"

//HeightMap
class MeshMap;
class Interface;
typedef std::string SkillPrimaryKey;
typedef float SkillTimeStamp;
typedef float SkillCoolTime;
class Character
{
protected:
	struct ToViewSpaceTransformData
	{
		TMatrix matWorld;
		TMatrix matView;
		TMatrix matProj;
	};
	ToViewSpaceTransformData _toViewSpaceTransformData;
	ID3D11Buffer* _toViewSpaceTransformBuffer;

	struct ObjectToWorldTransformData
	{
		TMatrix World;
	};
	ObjectToWorldTransformData _objectToWorldTransformData;
	ID3D11Buffer* _objectToWorldTransformBuffer;

public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	
public:
	Interface* m_pInterGageHP;
	Interface* m_pInterDamage;
	Interface* m_pInterMinimapProfile;

public:
	Character(float maxHealth = 100);
	virtual ~Character() {}

public:
	virtual bool	Init();
	virtual bool	Frame();
	virtual bool PreRender();
	virtual bool	Render();
	virtual bool	Release();

	void	UpdateBox();

	void	UpdateMatrix();
	virtual void	SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj);

	void	SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void	CreateBuffer();
	void	UpdateBuffer();
	SSB::OBBData GetBoundingVolume();

	void Scale(float val);

	TVector3 GetCurSocketPos(std::string socket);

public:
	float   m_fSpeed = 15;
	TVector3 m_vDirection;
	XMVECTOR m_vOldDirection;

public:
	virtual void	SetMap(MeshMap* pMap);
	MeshMap* m_pMap;

public:
	void    MoveChar(XMVECTOR& destinationDirection, XMMATRIX& worldMatrix, bool ghost = false);
	void    MoveChar(XMVECTOR& destinationDirection, XMMATRIX& worldMatrix, float speed, bool ghost = false);

public:
	SSB::Model* m_pModel = nullptr;

	TVector3	m_vPos = TVector3::Zero;
	TVector3	m_vRotation = TVector3::Zero;
	TVector3	m_vScale = TVector3::One;

	TMatrix		m_matWorld = TMatrix::Identity;
	TMatrix		m_matView = TMatrix::Identity;
	TMatrix		m_matProj = TMatrix::Identity;

public:
	T_BOX m_ColliderBox;

public:
	void Initialize_SetPosition(TVector3 pos);

public:
	TVector3 GetPosition();
	bool IsDead();

public:
	TMatrix m_AttackBoxLocalMatrix;
	T_BOX   m_AttackBox;

public:
	const int m_kHealthPointMax = 100;
	int m_HealthPoint = 100;
	int m_Damage = 30;

private:
	Sound* _damagedSound = nullptr;

public:
	virtual void Damage(int damage);

private:
	std::map<SkillPrimaryKey, SkillCoolTime> _skillCoolTimeList;
	std::map<SkillPrimaryKey, SkillTimeStamp> _skillTimeStampList;

public:
    void Initialize_RegisterSkill(SkillPrimaryKey key, SkillCoolTime coolTime);
	bool IsCoolTimePassed(SkillPrimaryKey key, time_t elaspedGameTime);
	SkillTimeStamp GetRemainSkillCoolTime(SkillPrimaryKey key);
    SkillCoolTime GetSkillCoolTime(SkillPrimaryKey key);
    void ActiveSkill(SkillPrimaryKey key);

public:
	void SetCurrentAnimation(SSB::AnimationName animationName);

private:
	float _airborneTimeStamp = 0;
	const float _kAirborneActiveTime = 1.0f;
	bool _airborne = false;

public:
	void SetAirborne();
	bool IsAirborne();
	float GetAirborneActiveTime();

private:
	bool _pound = false;

public:
	void SetPoundState(bool state);
	bool IsPoundState();

};

