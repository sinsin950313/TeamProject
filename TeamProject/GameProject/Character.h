#pragma once

#include "Model.h"
#include "TSelect.h"
#include <map>
#include "Sound.h"

//HeightMap
class MeshMap;

class Character
{
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
	virtual bool	Init();
	virtual bool	Frame();
	virtual bool	Render();
	virtual bool	Release();

	void	UpdateBox();

	void	UpdateMatrix();
	void	SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj);

	void	SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void	CreateBuffer();
	void	UpdateBuffer();
	SSB::OBBData GetBoundingVolume();

	void Scale(float val);

	TVector3 GetCurSocketPos(std::string socket);

	Sound* _currentSound = nullptr;
	Sound* _damagedSound = nullptr;

public:
    float   m_fSpeed = 15;
    TVector3 m_vDirection;
	XMVECTOR m_vOldDirection;

public:
	void	SetMap(MeshMap* pMap);
	MeshMap* m_pMap;
public:
    void    MoveChar(XMVECTOR& destinationDirection, XMMATRIX& worldMatrix);

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
	int m_HealthPoint = 100;
	int m_Damage = 30;
	std::set<Character*> m_DamagedCharacters;

public:
	void DamagingCharacter(Character* character);
	bool IsAlreadyDamagedCurrentState(Character* character);
	void Damage(int damage);

public:
	float m_fStateTImeStamp = 0;
	float m_fBeforeTime = 0;
	float m_fStateElapseTime = 0;

public:
	void ResetStateElapseTime();
	float GetStateElapseTime();
	float GetStateTimeStamp();
};

