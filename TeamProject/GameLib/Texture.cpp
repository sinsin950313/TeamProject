#include "Texture.h"

bool	Texture::Load(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, std::wstring name)
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(
		pd3dDevice,
		pContext,
		name.c_str(),
		(ID3D11Resource**)&m_pTexture,
		&m_pTextureSRV);
	if (FAILED(hr))
	{
		hr = DirectX::CreateDDSTextureFromFile(
			pd3dDevice,
			pContext,
			name.c_str(),
			(ID3D11Resource**)&m_pTexture,
			&m_pTextureSRV);
	}
	if (FAILED(hr))
		return false;

	m_pTexture->GetDesc(&m_Desc);

	return true;
}

void	Texture::Apply(ID3D11DeviceContext* pContext, UINT iSlot)
{
	pContext->PSSetShaderResources(iSlot, 1, &m_pTextureSRV);
}

bool	Texture::Release()
{
	if (m_pTexture)m_pTexture->Release();
	if (m_pTextureSRV)m_pTextureSRV->Release();
	return true;
}