#pragma once
#include "Model.h"
class ModelTest
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
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	void	SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj);
	

	void	SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	{
		m_pd3dDevice = pDevice;
		m_pImmediateContext = pContext;
	}

public:
	SSB::Model* m_pModel = nullptr;
};

