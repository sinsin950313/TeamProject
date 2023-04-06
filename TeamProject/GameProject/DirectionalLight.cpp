#include "DirectionalLight.h"
#include "ShaderMgr.h"
#include "CommonPath.h"

namespace SSB
{
	HRESULT DirectionalLight::CreateLightingShader()
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
			(kShaderPath + L"DirectionalLightShader.hlsl").c_str(),
			defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"GS",
			"gs_5_0",
			dwShaderFlags,
			0,
			&m_pGSCode,
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
		hr = _device->CreateGeometryShader(
			m_pGSCode->GetBufferPointer(),
			m_pGSCode->GetBufferSize(),
			NULL,
			&_lightingShader);

		return hr;
	}
}
