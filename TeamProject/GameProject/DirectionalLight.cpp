#include "DirectionalLight.h"
#include "ShaderMgr.h"

namespace SSB
{
	Shader* DirectionalLight::GetLightingShader()
	{
		Shader* shader = nullptr;
		// Light really need PS?
		//I_Shader.Load(L"DirectionalLightShader.hlsl", L"VS", L"PS", &shader);
		return shader;
	}
}
