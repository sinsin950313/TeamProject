#include "Sprite.h"
#include "DXState.h"
#include "Input.h"

Sprite::Sprite()
{

}

Sprite::~Sprite()
{
}

bool Sprite::Init()
{
	BaseObject::Init();

	I_Shader.VSLoad(L"../../data/shader/DefaultParticle.hlsl", L"VS", &m_pSwapVS[0]);
	I_Shader.VSLoad(L"../../data/shader/DefaultParticle.hlsl", L"RibbonVS", &m_pSwapVS[1]);

	I_Shader.PSLoad(L"../../data/shader/DefaultParticle.hlsl", L"PS", &m_pSwapPS[0]);
	I_Shader.PSLoad(L"../../data/shader/DefaultParticle.hlsl", L"Distortion", &m_pSwapPS[1]);
	I_Shader.PSLoad(L"../../data/shader/DefaultParticle.hlsl", L"COLOR_PS", &m_pSwapPS[2]);
	I_Shader.PSLoad(L"../../data/shader/DefaultParticle.hlsl", L"OPACITY_PS", &m_pSwapPS[3]);

	m_pSwapGS[0] = NULL;
	I_Shader.GSLoad(L"../../data/shader/DefaultParticle.hlsl", L"RibbonGS", &m_pSwapGS[1]);


	if (m_pTexture)
	{
		FLOAT dsWidth = m_pTexture->m_Desc.Width / 4;
		FLOAT dsHeight = m_pTexture->m_Desc.Height / 4;
		RenderTarget DownSampleRT;
		DownSampleRT.Create(m_pd3dDevice, m_pImmediateContext, dsWidth, dsHeight);

		RenderTarget Hor;
		Hor.Create(m_pd3dDevice, m_pImmediateContext, dsWidth, dsHeight);

		RenderTarget Ver;
		Ver.Create(m_pd3dDevice, m_pImmediateContext, dsWidth, dsHeight);

		if (m_EmissionRT)
		{
			m_EmissionRT->Release();
			delete m_EmissionRT;
			m_EmissionRT = nullptr;
		}

		m_EmissionRT = new RenderTarget();
		m_EmissionRT->Create(m_pd3dDevice, m_pImmediateContext, m_pTexture->m_Desc.Width, m_pTexture->m_Desc.Height);

		m_cbData.vTemp.x = dsWidth;
		m_cbData.vTemp.y = dsHeight;
		UpdateConstantBuffer();

		Shader* RenderShader;
		I_Shader.Load(L"../../data/shader/DefaultBlur.hlsl", L"VS", L"PS", &RenderShader);

		Shader* HBlurShader;
		I_Shader.Load(L"../../data/shader/DefaultBlur.hlsl", L"H_VS", L"H_PS", &HBlurShader);

		Shader* VBlurShader;
		I_Shader.Load(L"../../data/shader/DefaultBlur.hlsl", L"V_VS", L"V_PS", &VBlurShader);

		UINT m_nViewPorts = 1;
		/*
		m_pImmediateContext->RSGetViewports(&m_nViewPorts, m_EmissionRT->m_vpOld);
		m_pImmediateContext->OMGetRenderTargets(1, &m_EmissionRT->m_pOldRTV, &m_EmissionRT->m_pOldDSV);
		*/
		TColor color = TColor(0, 0, 0, 0);
		if (DownSampleRT.Begin(m_pImmediateContext, color))
		{
			UINT stride = sizeof(Vertex);
			UINT offset = 0;

			m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
			m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			m_pImmediateContext->VSSetShader(RenderShader->m_pVS, NULL, 0);
			m_pImmediateContext->PSSetShader(RenderShader->m_pPS, NULL, 0);

			m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

			m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureSRV);

			m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

			BaseObject::PostRender();
			DownSampleRT.End(m_pImmediateContext);
		}
		if (Hor.Begin(m_pImmediateContext, color))
		{
			m_pImmediateContext->VSSetShader(HBlurShader->m_pVS, NULL, 0);
			m_pImmediateContext->PSSetShader(HBlurShader->m_pPS, NULL, 0);
			m_pImmediateContext->PSSetShaderResources(0, 1, DownSampleRT.m_pSRV.GetAddressOf());
			BaseObject::PostRender();
			Hor.End(m_pImmediateContext);
		}
		if (Ver.Begin(m_pImmediateContext, color))
		{
			m_pImmediateContext->VSSetShader(VBlurShader->m_pVS, NULL, 0);
			m_pImmediateContext->PSSetShader(VBlurShader->m_pPS, NULL, 0);
			m_pImmediateContext->PSSetShaderResources(0, 1, Hor.m_pSRV.GetAddressOf());
			BaseObject::PostRender();

			Ver.End(m_pImmediateContext);
		}
		m_cbData.vTemp.x = m_pTexture->m_Desc.Width;
		m_cbData.vTemp.y = m_pTexture->m_Desc.Height;
		UpdateConstantBuffer();
		if (m_EmissionRT->Begin(m_pImmediateContext, color))
		{
			m_pImmediateContext->VSSetShader(RenderShader->m_pVS, NULL, 0);
			m_pImmediateContext->PSSetShader(RenderShader->m_pPS, NULL, 0);
			m_pImmediateContext->PSSetShaderResources(0, 1, Ver.m_pSRV.GetAddressOf());
			m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

			BaseObject::PostRender();

			m_EmissionRT->End(m_pImmediateContext);
		}
	}


	return true;
}

bool Sprite::Frame()
{
	// UVRectList의 개수로 UV애니메이션 / 텍스쳐 애니메이션 구분
	// 프레임은 받아야

	//UpdateMatrix();
	BaseObject::Frame();

	//static int curMat = 0;
	//if (I_Input.GetKey('I') == KEY_PUSH)
	//{
	//	TVector3 pos = GetRandPos();
	//	D3DXMatrixTranslation(&m_matInstancing[curMat++], pos.x, pos.y, pos.z);
	//}
	
	return true;
}
bool Sprite::PreRender()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_pImmediateContext->VSSetShader(m_pVS ? m_pVS->m_pVS : NULL, NULL, 0);
	m_pImmediateContext->GSSetShader(m_pGS ? m_pGS->m_pGS : NULL, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pPS ? m_pPS->m_pPS : NULL, NULL, 0);

	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureSRV);

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->RSSetState(DXState::g_pNonCullRSSolid);
	return true;
}

bool Sprite::Render()
{
	PreRender();
	PostRender();
	return true;
}

bool Sprite::RenderInstancing(UINT size)
{
	PreRender();

	if (m_pIndexBuffer == nullptr)
		m_pImmediateContext->DrawInstanced(m_VertexList.size(), size, 0, 0);
	else
		m_pImmediateContext->DrawIndexedInstanced(m_IndexList.size(), size, 0, 0, 0);

	m_pImmediateContext->RSSetState(DXState::g_pDefaultRSSolid);
	return true;
}

bool Sprite::PostRender()
{
	BaseObject::PostRender();
	m_pImmediateContext->RSSetState(DXState::g_pDefaultRSSolid);
	return true;
}

bool Sprite::Release()
{
	if (m_EmissionRT)
	{
		m_EmissionRT->Release();
		delete m_EmissionRT;
		m_EmissionRT = nullptr;
	}
	BaseObject::Release();
	return true;
}

void	Sprite::SetBlurData(bool isBlur)
{
	if (isBlur)
	{
		if (m_EmissionRT)
			m_pImmediateContext->PSSetShaderResources(11, 1, m_EmissionRT->m_pSRV.GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* pSRV = 0;
		m_pImmediateContext->PSSetShaderResources(11, 1, &pSRV);
	}
}