#pragma once
#include "Model.h"
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

	void	UpdateMatrix();
	void	SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj);

	void	SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void	CreateBuffer();
	void	UpdateBuffer();
	SSB::OBBData GetBoundingVolume();

public:
	SSB::Model* m_pModel = nullptr;

	TVector3	m_vPos = TVector3::Zero;
	TVector3	m_vRotation = TVector3::Zero;
	TVector3	m_vScale = TVector3::One;

	TMatrix		m_matWorld = TMatrix::Identity;
	TMatrix		m_matView = TMatrix::Identity;
	TMatrix		m_matProj = TMatrix::Identity;
};

