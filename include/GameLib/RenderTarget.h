#pragma once
#include "BaseObject.h"
class RenderTarget
{
public:
	ComPtr<ID3D11Texture2D> m_pDSTexture = nullptr;
	ComPtr<ID3D11RenderTargetView>		m_pRenderTargetView;
	ComPtr<ID3D11DepthStencilView>		m_pDepthStencilView;
	ComPtr<ID3D11ShaderResourceView>	m_pSRV;
	ComPtr<ID3D11ShaderResourceView>	m_pDsvSRV;// ���̹��� ���ҽ���
	ComPtr<ID3D11Texture2D>		m_pTexture;
	D3D11_DEPTH_STENCIL_VIEW_DESC	m_DepthStencilDesc;
	D3D11_TEXTURE2D_DESC	m_TexDesc;
	D3D11_VIEWPORT	m_Viewport;

	ID3D11RenderTargetView* m_pOldRTV;
	ID3D11RenderTargetView** m_pOldRTVS;
	UINT m_iOldRTVCount;
	ID3D11DepthStencilView* m_pOldDSV;
	D3D11_VIEWPORT	m_vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX] = { 0, };

	bool	m_bWireFrame = false;

public:
	bool	Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, FLOAT fWidth = 1024.0f, FLOAT fHeight = 1024.0f);
	bool	Begin(ID3D11DeviceContext* pContext, TColor color = TColor(0, 0, 0, 0));
	void	End(ID3D11DeviceContext* pContext);
	bool	Release();
};
