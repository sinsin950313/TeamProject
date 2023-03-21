#include "Shader.h"

bool	Shader::Init()
{
	return true;
}

bool	Shader::Frame()
{
	return true;
}

bool	Shader::Render()
{
	return true;
}

bool	Shader::Release()
{
	if (m_pVS)m_pVS->Release();
	if (m_pPS)m_pPS->Release();
	if (m_pVSCode) m_pVSCode->Release();
	if (m_pPSCode)m_pPSCode->Release();
	return true;
}

HRESULT Shader::Load(ID3D11Device* pd3dDevice,	ID3D11DeviceContext* pContext, std::wstring name, std::wstring VSname, std::wstring PSname)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;
	HRESULT hr;
	hr = CreateVertexShader(name, VSname);
	if (FAILED(hr))
		return hr;
	hr = CreatePixelShader(name, PSname);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT Shader::VSLoad(
	ID3D11Device* pd3dDevice,
	ID3D11DeviceContext* pContext,
	std::wstring name, std::wstring VSname)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;
	HRESULT hr;
	hr = CreateVertexShader(name, VSname);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT Shader::PSLoad(
	ID3D11Device* pd3dDevice,
	ID3D11DeviceContext* pContext,
	std::wstring name, std::wstring PSname)
{
	m_pd3dDevice = pd3dDevice;
	m_pImmediateContext = pContext;
	HRESULT hr;
	hr = CreatePixelShader(name, PSname);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT Shader::CreateVertexShader(std::wstring name, std::wstring VSname)
{
	ID3DBlob* pErrorCode = nullptr;
	DWORD dwShaderFlags = 0;
#ifdef _DEBUG
	dwShaderFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
#endif

	std::string str = std::to_string(MAXLIGHTNUM);
	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		"MAXLIGHTNUM", str.c_str(),
		NULL, NULL
	};

	HRESULT hr = D3DCompileFromFile(
		name.c_str(),
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		wtm(VSname).c_str(),
		"vs_5_0",
		dwShaderFlags,
		0,
		&m_pVSCode,
		&pErrorCode);
	if (FAILED(hr))
	{
		if (pErrorCode)
		{
			OutputDebugStringA((char*)pErrorCode->GetBufferPointer());
			pErrorCode->Release();
		}
		return hr;
	}
	hr = m_pd3dDevice->CreateVertexShader(
		m_pVSCode->GetBufferPointer(),
		m_pVSCode->GetBufferSize(),
		NULL,
		&m_pVS);

	return hr;
}

HRESULT Shader::CreatePixelShader(std::wstring name, std::wstring PSname)
{
	ID3DBlob* pErrorCode = nullptr;
	DWORD dwShaderFlags = 0;
#ifdef _DEBUG
	dwShaderFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
#endif

	std::string str = std::to_string(MAXLIGHTNUM);
	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		"MAXLIGHTNUM", str.c_str(),
		NULL, NULL
	};

	HRESULT hr = D3DCompileFromFile(
		name.c_str(),
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		wtm(PSname).c_str(),
		"ps_5_0",
		dwShaderFlags,
		0,
		&m_pPSCode,
		&pErrorCode);
	if (FAILED(hr))
	{
		if (pErrorCode)
		{
			OutputDebugStringA((char*)pErrorCode->GetBufferPointer());
			pErrorCode->Release();
		}
		return hr;
	}
	hr = m_pd3dDevice->CreatePixelShader(
		m_pPSCode->GetBufferPointer(),
		m_pPSCode->GetBufferSize(),
		NULL,
		&m_pPS);
	if (FAILED(hr))
	{
		if (pErrorCode)
		{
			OutputDebugStringA((char*)pErrorCode->GetBufferPointer());
			pErrorCode->Release();
		}
		return hr;
	}

	return hr;
}