#pragma once
#include "DXDevice.h"
#include "TextureMgr.h"
#include "ShaderMgr.h"

struct Vertex
{
	TVector3 p;
	TVector3 n;
	TVector4 c;
	TVector2 t;
};

struct VS_CONSTANT_BUFFER
{
	TMatrix matWorld;
	TMatrix matView;
	TMatrix matProj;
	TMatrix matComb;
	TMatrix matNormal;
	TVector3 vTemp;
	float	fTimer;
};

namespace DX
{
	ID3D11Buffer* CreateVertexBuffer(ID3D11Device* pd3dDevice,
		void* pDataAddress,
		UINT iNumVertex,
		UINT iVertexSize);

	ID3D11Buffer* CreateIndexBuffer(ID3D11Device* pd3dDevice,
		void* pDataAddress,
		UINT iDataNum,
		UINT iDataSize);

	ID3D11Buffer* CreateConstantBuffer(ID3D11Device* pd3dDevice,
		void* pDataAddress,
		UINT iDataSize);

	class BaseObject
	{
	public:
		ID3D11Device* m_pd3dDevice = nullptr;
		ID3D11DeviceContext* m_pImmediateContext = nullptr;

	public:
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
		ID3D11InputLayout* m_pVertexLayout;

		ID3D11Buffer* m_pConstantBuffer;
		VS_CONSTANT_BUFFER m_cbData;

	public:
		std::vector<Vertex> m_VertexList;
		std::vector<DWORD> m_IndexList;

	public:
		Texture* m_pTexture = nullptr;
		ID3D11ShaderResourceView* m_pTextureSRV = nullptr;
		Shader* m_pShader = nullptr;
		std::wstring m_szTexPath;
		std::wstring m_szShaderPath;
		std::wstring m_szVSname;
		std::wstring m_szPSname;

	public:
		TMatrix m_matWorld;
		TMatrix m_matView;
		TMatrix m_matProj;

		TVector3 m_vScale;
		TVector3 m_vRotation;
		TVector3 m_vPos;

	public:
		virtual bool	Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, std::wstring shaderPath, std::wstring texPath,
			std::wstring VSname = L"VS", std::wstring PSname = L"PS");
		virtual void	CreateVertexData();
		virtual void	CreateIndexData();
		virtual void	CreateConstantData();
		virtual HRESULT CreateVertexBuffer();
		virtual HRESULT CreateIndexBuffer();
		virtual HRESULT CreateConstantBuffer();
		virtual HRESULT CreateVertexLayout();
		virtual void	UpdateVertexBuffer();
		virtual void	UpdateConstantBuffer();

		virtual void	SetMatrix(TMatrix* matWorld, TMatrix* matView, TMatrix* matProj);

	public:
		virtual bool Init();
		virtual bool Frame();
		virtual bool PreRender();
		virtual bool Render();
		virtual bool PostRender();
		virtual bool Release();
	};
};
using namespace DX;

