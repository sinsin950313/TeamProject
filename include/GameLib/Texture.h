#pragma once
#include "std.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTK_D.lib")
#else
#pragma comment(lib, "DirectXTK_R.lib")
#endif


class Texture
{
public:
	ID3D11Texture2D* m_pTexture = nullptr;
	ID3D11ShaderResourceView* m_pTextureSRV = nullptr;
	D3D11_TEXTURE2D_DESC	m_Desc;

public:
	bool	Load(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, std::wstring name);
	void	Apply(ID3D11DeviceContext* pContext, UINT iSlot = 0);
	bool	Release();
};

