#include "DXState.h"

ID3D11SamplerState* DXState::g_pDefaultSS = nullptr;
ID3D11SamplerState* DXState::g_pDefaultSSMirror = nullptr;

ID3D11BlendState* DXState::g_pAlphaBlend = nullptr;
ID3D11BlendState* DXState::g_pAddAlphaBlend = nullptr;

ID3D11RasterizerState* DXState::g_pDefaultRSWireFrame = nullptr;
ID3D11RasterizerState* DXState::g_pNonCullRSSolid = nullptr;
ID3D11RasterizerState* DXState::g_pDefaultRSSolid = nullptr;

ID3D11DepthStencilState* DXState::g_pDefaultDepthStencil = nullptr;
ID3D11DepthStencilState* DXState::g_pDefaultDepthStencilAndNoWrite = nullptr;
ID3D11DepthStencilState * DXState::g_pGreaterDepthStencil = nullptr;

bool DXState::SetState(ID3D11Device* pd3dDevice)
{
	D3D11_SAMPLER_DESC sd;
    ZeroMemory(&sd, sizeof(sd));

    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    //D3D11_TEXTURE_ADDRESS_WRAP = 1,
    //D3D11_TEXTURE_ADDRESS_MIRROR = 2,
    //D3D11_TEXTURE_ADDRESS_CLAMP = 3,
    //D3D11_TEXTURE_ADDRESS_BORDER = 4,
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

    sd.BorderColor[0] = sd.BorderColor[1] = sd.BorderColor[2] = 0.5f; // BORDER 모드의 보더 색상
    sd.BorderColor[3] = 1.0f;

    //FLOAT MipLODBias;
    //UINT MaxAnisotropy;
    //D3D11_COMPARISON_FUNC ComparisonFunc;
    //FLOAT BorderColor[4];
    //FLOAT MinLOD;
    //FLOAT MaxLOD;
	pd3dDevice->CreateSamplerState(&sd, &g_pDefaultSS);

    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
    pd3dDevice->CreateSamplerState(&sd, &g_pDefaultSSMirror);

    D3D11_RASTERIZER_DESC rd;
    ZeroMemory(&rd, sizeof(rd));
    rd.DepthClipEnable = TRUE;
    rd.FillMode = D3D11_FILL_WIREFRAME;
    rd.CullMode = D3D11_CULL_BACK;
    pd3dDevice->CreateRasterizerState(&rd, &g_pDefaultRSWireFrame);

    rd.FillMode = D3D11_FILL_SOLID;
    pd3dDevice->CreateRasterizerState(&rd, &g_pDefaultRSSolid);

    rd.CullMode = D3D11_CULL_NONE;
    pd3dDevice->CreateRasterizerState(&rd, &g_pNonCullRSSolid);

    D3D11_BLEND_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.RenderTarget[0].BlendEnable = TRUE;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    pd3dDevice->CreateBlendState(&bd, &g_pAlphaBlend);

    bd.AlphaToCoverageEnable = FALSE;
    bd.IndependentBlendEnable = TRUE;
    bd.RenderTarget[0].BlendEnable = TRUE;

    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;		 // SV_TARGET0
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_SRC1_COLOR; // SV_TARGET1
    //DualSourceColorBlending에서 ***_SRC1_*** 은 SV_TARGET1으로 사용된다.

    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    pd3dDevice->CreateBlendState(&bd, &g_pAddAlphaBlend);

    D3D11_DEPTH_STENCIL_DESC dsd;
    ZeroMemory(&dsd, sizeof(dsd));
    dsd.DepthEnable = TRUE;
    // 0.5f * mask(1) -> output(0.5) ?? 곱해서 0.5가 나온건지 잘려서 0.5가 나온건지 불확실
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;  // 깊이를 판단할 기준 함수? 뭐보다 작은가 큰가 같은가
    /*
    BOOL StencilEnable;
    UINT8 StencilReadMask;
    UINT8 StencilWriteMask;
    D3D11_DEPTH_STENCILOP_DESC FrontFace;
    D3D11_DEPTH_STENCILOP_DESC BackFace; 
    */
    pd3dDevice->CreateDepthStencilState(&dsd, &g_pDefaultDepthStencil);
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    pd3dDevice->CreateDepthStencilState(&dsd, &g_pDefaultDepthStencilAndNoWrite);
    dsd.DepthFunc = D3D11_COMPARISON_GREATER; 
    pd3dDevice->CreateDepthStencilState(&dsd, &g_pGreaterDepthStencil);

	return true;
}

bool DXState::Release()
{
    if (g_pDefaultSS)
    {
        g_pDefaultSS->Release();
        g_pDefaultSS = nullptr;
    }
    if (g_pDefaultSSMirror)
    {
        g_pDefaultSSMirror->Release();
        g_pDefaultSSMirror = nullptr;
    }
    if (g_pDefaultRSWireFrame)
    {
        g_pDefaultRSWireFrame->Release();
        g_pDefaultRSWireFrame = nullptr;
    }
    if (g_pNonCullRSSolid)
    {
        g_pNonCullRSSolid->Release();
        g_pNonCullRSSolid = nullptr;
    }
    if (g_pDefaultRSSolid)
    {
        g_pDefaultRSSolid->Release();
        g_pDefaultRSSolid = nullptr;
    }
    if (g_pAlphaBlend)
    {
        g_pAlphaBlend->Release();
        g_pAlphaBlend = nullptr;
    }
    if (g_pAddAlphaBlend)
    {
        g_pAddAlphaBlend->Release();
        g_pAddAlphaBlend = nullptr;
    }
    if (g_pDefaultDepthStencil)
    {
        g_pDefaultDepthStencil->Release();
        g_pDefaultDepthStencil = nullptr;
    }
    if (g_pDefaultDepthStencilAndNoWrite)
    {
        g_pDefaultDepthStencilAndNoWrite->Release();
        g_pDefaultDepthStencilAndNoWrite = nullptr;
    }
    if (g_pGreaterDepthStencil)
    {
        g_pGreaterDepthStencil->Release();
        g_pGreaterDepthStencil = nullptr;
    }

	return true;
}
