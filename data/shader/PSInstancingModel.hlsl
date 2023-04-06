#include "SSB/PixelShaderInputType.hlsli"

float4 PS(PixelShaderInput_PCNT input) : SV_TARGET
{
	float4 tex = Diffuse.Sample(Sampler, input.Diffuse);
	return tex * input.Color;
}
