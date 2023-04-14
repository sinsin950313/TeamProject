#include "PixelShaderInputType.hlsli"
#include "PixelShaderOutputType.hlsli"

PixelOutput PS(PixelShaderInput_PCNT input)
{
	//float4 tex = Diffuse.Sample(Sampler, input.Diffuse);
	//if(tex.a < 0.5f)
	//	discard;
	//return tex;

	PixelOutput output = (PixelOutput)0;

	output.Position = input.World;
	output.Normal = input.Normal;
	output.Color = Diffuse.Sample(Sampler, input.Diffuse);
	if(output.Color.w < 0.1)
		discard;

	return output;
}
