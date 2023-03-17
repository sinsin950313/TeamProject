#include "DXDevice.h"

HRESULT DXDevice::ResizeDevice(UINT width, UINT height)
{
    HRESULT hr;
    // 윈도우 크기 변경 메시시 WM_SIZE
    if (m_pd3dDevice == nullptr) return S_OK;

    // 현재 설정된 랜더타겟 해제 및 소멸
    DeleteDxResource();
    m_pImmediateContext->OMSetRenderTargets(0, nullptr, NULL);
    m_pRTV->Release();
    m_pDepthStencilView->Release();
    //delete m_pRTV;

    // 변경된 윈도우의 크기를 얻고 백 버퍼의 크기를 재 조정
    // 백버퍼의 크기를 조정한다
    GetWindowRect(g_hWnd, &g_rcWindow);
    GetClientRect(g_hWnd, &g_rcClient);
    DXGI_SWAP_CHAIN_DESC CurrentSD, AfterSD;
    m_pSwapChain->GetDesc(&CurrentSD);
    hr = m_pSwapChain->ResizeBuffers(CurrentSD.BufferCount, width, height, CurrentSD.BufferDesc.Format, 0);

    // 변경된 백 버퍼의 크기를 얻고 랜더타겟 뷰를 다시 생성 및 적용
    // 뷰포트 재조정
    if (FAILED(hr = CreateRenderTargetView()))
    {
        return E_FAIL;
    }
    if (FAILED(hr = CreateDepthStencilView()))
    {
        return E_FAIL;
    }
    CreateViewport();

    CreateDxResource();
    return S_OK;
}

HRESULT DXDevice::CreateDxResource()
{
    return S_OK;
}

HRESULT DXDevice::DeleteDxResource()
{
    return S_OK;
}

bool    DXDevice::Init()
{
    if (FAILED(CreateDevice()))
        return false;

    if (FAILED(CreateDXGIDevice()))
        return false;

    if (FAILED(CreateSwapChain()))
        return false;

    if (FAILED(CreateRenderTargetView()))
        return false;

    if (FAILED(CreateDepthStencilView()))
        return false;

    CreateViewport();


    return true;
}
bool    DXDevice::Frame()
{
    return true;
}
bool    DXDevice::Render()
{
    return true;
}
bool    DXDevice::Release()
{
    if (m_pd3dDevice) m_pd3dDevice->Release();
    if (m_pImmediateContext) m_pImmediateContext->Release();
    if (m_pGIFactory) m_pGIFactory->Release();
    if (m_pSwapChain) m_pSwapChain->Release();
    if (m_pRTV) m_pRTV->Release();
    if (m_pDepthStencilView)m_pDepthStencilView->Release();
    return true;
}

HRESULT DXDevice::CreateDevice()
{
	// 1) 디바이스 생성
	D3D_FEATURE_LEVEL pFeatureLevel;
	UINT Flags = 0;
	D3D_FEATURE_LEVEL pFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_9_3,
	};
	UINT FeatureLevels = sizeof(pFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        createDeviceFlags,
        pFeatureLevels,
        FeatureLevels,
        D3D11_SDK_VERSION,
        &m_pd3dDevice,
        &pFeatureLevel,
        &m_pImmediateContext
    );

    return hr;
}

HRESULT DXDevice::CreateDXGIDevice()
{
    // 2) 팩토리 생성
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pGIFactory);

	return hr;
}

HRESULT DXDevice::CreateSwapChain()
{
    // 3) 스왑체인 생성
    // 후면(백)버퍼 생성
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;

    sd.BufferDesc.Width = g_rcClient.right;
    sd.BufferDesc.Height = g_rcClient.bottom;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;

    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    sd.OutputWindow = g_hWnd;
    sd.Windowed = true;

    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;

    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    HRESULT hr = m_pGIFactory->CreateSwapChain(m_pd3dDevice, &sd, &m_pSwapChain);

    return hr;
}

HRESULT DXDevice::CreateRenderTargetView()
{
    // 4) 랜더타겟뷰 생성
    ID3D11Texture2D* pBackBuffer = nullptr;
    m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
    HRESULT hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRTV);
    pBackBuffer->Release();

    return hr;
}

HRESULT DXDevice::CreateDepthStencilView()
{
    HRESULT hr;

    D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    m_pRTV->GetDesc(&rtvd);
    DXGI_SWAP_CHAIN_DESC scd;
    m_pSwapChain->GetDesc(&scd);

    // 1번 - 텍스쳐를 생성한다.
    ID3D11Texture2D* pDSTexture;
    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(td));
    td.Width = scd.BufferDesc.Width;
    td.Height = scd.BufferDesc.Height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R24G8_TYPELESS;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = m_pd3dDevice->CreateTexture2D(&td, NULL, &pDSTexture);

    // 2번 - 깊이 스텐실 뷰로 생성한다.
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    ZeroMemory(&dsvd, sizeof(dsvd));
    dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    hr = m_pd3dDevice->CreateDepthStencilView(pDSTexture, &dsvd, &m_pDepthStencilView);
    pDSTexture->Release();
    // 3번 - 뷰 적용은 PreRender에서?
    //m_pImmediateContext->OMSetRenderTargets(1, &m_pRTV, &m_pDepthStencilView);


    return hr;
}

void DXDevice::CreateViewport()
{
    // 5) 뷰포트 설정
    m_vp.Width = g_rcClient.right;
    m_vp.Height = g_rcClient.bottom;
    m_vp.TopLeftX = 0;
    m_vp.TopLeftY = 0;

    m_vp.MinDepth = 0.0f;
    m_vp.MaxDepth = 1.0f;
    m_pImmediateContext->RSSetViewports(1, &m_vp);
}

void DXDevice::ClearD3D11DeviceContext()
{
    // Unbind all objects from the immediate context
    if (m_pImmediateContext == NULL) return;

    ID3D11ShaderResourceView* pSRVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11RenderTargetView* pRTVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11DepthStencilView* pDSV = NULL;
    ID3D11Buffer* pBuffers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11SamplerState* pSamplers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    UINT StrideOffset[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    // Shaders
    m_pImmediateContext->VSSetShader(NULL, NULL, 0);
    m_pImmediateContext->HSSetShader(NULL, NULL, 0);
    m_pImmediateContext->DSSetShader(NULL, NULL, 0);
    m_pImmediateContext->GSSetShader(NULL, NULL, 0);
    m_pImmediateContext->PSSetShader(NULL, NULL, 0);

    //// IA clear
    m_pImmediateContext->IASetVertexBuffers(0, 16, pBuffers, StrideOffset, StrideOffset);
    m_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);
    m_pImmediateContext->IASetInputLayout(NULL);

    //// Constant buffers
    m_pImmediateContext->VSSetConstantBuffers(0, 14, pBuffers);
    m_pImmediateContext->HSSetConstantBuffers(0, 14, pBuffers);
    m_pImmediateContext->DSSetConstantBuffers(0, 14, pBuffers);
    m_pImmediateContext->GSSetConstantBuffers(0, 14, pBuffers);
    m_pImmediateContext->PSSetConstantBuffers(0, 14, pBuffers);

    // Resources
    m_pImmediateContext->VSSetShaderResources(0, 16, pSRVs);
    m_pImmediateContext->HSSetShaderResources(0, 16, pSRVs);
    m_pImmediateContext->DSSetShaderResources(0, 16, pSRVs);
    m_pImmediateContext->GSSetShaderResources(0, 16, pSRVs);
    m_pImmediateContext->PSSetShaderResources(0, 16, pSRVs);

    //// Samplers
    //m_pImmediateContext->VSSetSamplers(0, 16, pSamplers);
    //m_pImmediateContext->HSSetSamplers(0, 16, pSamplers);
    //m_pImmediateContext->DSSetSamplers(0, 16, pSamplers);
    //m_pImmediateContext->GSSetSamplers(0, 16, pSamplers);
    //m_pImmediateContext->PSSetSamplers(0, 16, pSamplers);

    //// Render targets
    m_pImmediateContext->OMSetRenderTargets(8, pRTVs, pDSV);

    //// States
    //FLOAT blendFactor[4] = { 0,0,0,0 };
    //m_pImmediateContext->OMSetBlendState(NULL, blendFactor, 0xFFFFFFFF);
    //m_pImmediateContext->OMSetDepthStencilState(NULL, 0);
    //m_pImmediateContext->RSSetState(NULL);
}