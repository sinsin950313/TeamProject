#pragma once
#include "ShaderMgr.h"

struct PC_Vertex
{
	TVector3 p;
	TVector4 c;
};

struct BOX_CONSTANT_BUFFER
{
	TMatrix matView;
	TMatrix matProj;
	TMatrix matComb;
	TVector4 vXAxis;
	TVector4 vYAxis;
	TVector4 vZAxis;
	TVector4 vSize;
};

class DebugBox
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;

public:
	bool	Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	HRESULT	CreateShader(std::wstring filename = L"../../data/shader/DebugBoxShader.txt");

	HRESULT CreateVertexBuffer();
	HRESULT CreateConstantBuffer();
	HRESULT CreateVertexLayout();
	void		UpdateBuffer();

private:
	HRESULT	CreateGS(std::wstring filename);

public:
	void	SetBox(TVector3 pos, TVector3 rot, TVector3 size);
	void	SetColor(TColor color);
	void	SetMatrix(TMatrix* matView, TMatrix* matProj);


public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	Shader* m_pShader;
	ID3D11GeometryShader* m_pGS;
	ID3DBlob* m_pGSCode;

public:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pConstantBuffer;
	ID3D11InputLayout* m_pVertexLayout;

public:
	PC_Vertex	m_Vertex;
	BOX_CONSTANT_BUFFER m_cbBox;

public:
	TVector3 m_vPos;
	TVector3 m_vAxis[3];
	float	m_fExtent[3];
};

