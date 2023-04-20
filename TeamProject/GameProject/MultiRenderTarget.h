#pragma once

#include "std.h"

namespace SSB
{
	struct MultiRenderTargetResult
	{
		ID3D11ShaderResourceView* PositionRenderTargetView;
		ID3D11ShaderResourceView* NormalRenderTargetView;
		ID3D11ShaderResourceView* ColorRenderTargetView;
		ID3D11ShaderResourceView* DepthRenderTargetView;
	};

	class MultiRenderTarget
	{
	private:
		ID3D11Device* m_pd3dDevice = nullptr;
		ID3D11DeviceContext* m_pImmediateContext = nullptr;
		float m_fWidth;
		float m_fHeight;

	public:
		ID3D11Texture2D* m_pPositionTexture;
		ID3D11Texture2D* m_pNormalTexture;
		ID3D11Texture2D* m_pColorTexture;
		//ID3D11Texture2D* m_pDepthTexture;

	private:
		ID3D11RenderTargetView* m_pPositionRenderTargetView;
		ID3D11RenderTargetView* m_pNormalRenderTargetView;
		ID3D11RenderTargetView* m_pColorRenderTargetView;
		//ID3D11RenderTargetView* m_pDepthRenderTargetView;

	private:
		ID3D11ShaderResourceView* m_pPositionShaderResourceView;
		ID3D11ShaderResourceView* m_pNormalShaderResourceView;
		ID3D11ShaderResourceView* m_pColorShaderResourceView;
		//ID3D11ShaderResourceView* m_pDepthShaderResourceView;

	private:
		D3D11_VIEWPORT	m_Viewport;
		ID3D11BlendState* m_pBlendState = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11ShaderResourceView* m_pDepthStencilShaderResourceView;
		ID3D11PixelShader* m_pPixelShader = nullptr;

	private:
		void CreateViewPort();
		void CreateBlendState();
		HRESULT CreatePositionTexture();
		HRESULT CreateNormalTexture();
		HRESULT CreateColorTexture();
		//HRESULT CreateDepthTexture();
		HRESULT CreateDepthStencil();

	private:
		void ClearRenderTargets();

	public:
		bool	Initialize_SetData(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, FLOAT fWidth = 2048.0f, FLOAT fHeight = 2048.0f);
		MultiRenderTargetResult GetResult();

		ID3D11DepthStencilView* GetDepthStencilView() { return m_pDepthStencilView; }

	public:
		virtual bool Init();
		virtual bool Frame();
		virtual bool Render();
		virtual bool Release();
	};
}
