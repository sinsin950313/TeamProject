#pragma once

#include "TMath.h"
#include <vector>
#include "TextureMgr.h"
#include "ShaderMgr.h"
#include "RenderTarget.h"
#include "Light.h"

namespace SSB
{
	struct ScreenVertex
	{
		TVector3 p;
	};

	class Screen
	{
	private:
		ID3D11Device* m_pd3dDevice = nullptr;
		ID3D11DeviceContext* m_pImmediateContext = nullptr;

	private:
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
		ID3D11InputLayout* m_pVertexLayout;

	private:
		std::vector<ScreenVertex> m_VertexList;
		std::vector<DWORD> m_IndexList;

	private:
		Shader* m_pShader = nullptr;
		ID3D11Texture2D* m_pTexture = nullptr;
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

	public:
		bool	Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
		void	CreateVertexData();
		void	CreateIndexData();
		HRESULT CreateVertexBuffer();
		HRESULT CreateIndexBuffer();
		HRESULT CreateVertexLayout();

	public:
		void UpdateRenderTargetTextureData(RenderTargetResult result);
		void UpdateLightTextureData(std::vector<SSB::Light*> lights);

	public:
		virtual bool Init();
		virtual bool Frame();
		virtual bool PreRender();
		virtual bool Render();
		virtual bool PostRender();
		virtual bool Release();
	};
}
