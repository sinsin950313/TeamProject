#include "PixelShaderInputType.hlsli"

float4 PS(PixelShaderInput_PCNT input) : SV_TARGET
{
	float4 tex = Diffuse.Sample(Sampler, input.Diffuse);
	if(tex.a < 0.5f)
		discard;
	return tex;
}
