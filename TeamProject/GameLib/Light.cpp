#include "Light.h"

namespace SSB
{
	void Light::RegisterLightingPixelShader()
	{
		_lightingPixelShader = GetLightingPixelShader();
	}
	void Light::Initialize_SetDevice(ID3D11Device* device, ID3D11DeviceContext* dc)
	{
		_device = device;
		_dc = dc;
	}
	void Light::Frame()
	{
		UpdateDepthMap();
		UpdateLightBuffer();
	}
	void Light::PreRender()
	{
		_dc->OMSetRenderTargets(0, nullptr, _depthMapDepthStencilView);
	}
}