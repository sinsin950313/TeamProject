#pragma once

#include "TMath.h"
#include <vector>
#include "TextureMgr.h"
#include "ShaderMgr.h"
#include "MultiRenderTarget.h"
#include "Light.h"

namespace SSB
{
	struct ScreenVertex
	{
		TVector3 p;
		TVector2 uv;
	};

	class Screen
	{
	private:
		ID3D11Device* m_pd3dDevice = nullptr;
		ID3D11DeviceContext* m_pImmediateContext = nullptr;
		float m_fWidth;
		float m_fHeight;

	private:
		std::vector<ScreenVertex> m_VertexList;
		std::vector<DWORD> m_IndexList;

	private:
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
		ID3D11InputLayout* m_pVertexLayout;

	private:
		ID3D11Texture2D* m_pDepthStencilTexture;
		ID3D11DepthStencilView* m_pDepthStencilView;

	private:
		Shader* m_pShader = nullptr;
		ID3D11Texture2D* m_pTexture = nullptr;
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
		ID3D11BlendState* m_pLightBlendState = nullptr;
		ID3D11SamplerState* _pcfSamplerState;

	private:
		void CreatePCFComparisonState();
		void ClearRenderTarget();
		void	CreateVertexData();
		HRESULT CreateVertexBuffer();
		void	CreateIndexData();
		HRESULT CreateIndexBuffer();
		HRESULT CreateVertexLayout();
		HRESULT CreateDepthStencilData();
		HRESULT CreateRenderTargetData();
		void CreateShader();

	private:
		static const int m_ReferecedTextureCount = 4;
		ID3D11ShaderResourceView* m_ShaderResourceList[m_ReferecedTextureCount];

	public:
		void Initialize_SetData(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, FLOAT fWidth = 2048.0f, FLOAT fHeight = 2048.0f);
		void SetMultiRenderTargetResult(MultiRenderTargetResult result);

	public:
		bool Init();
		bool Frame();
		bool Render();
		bool Release();

	public:
		ID3D11Texture2D* GetRenderTargetTexture();
	};
}
