#include "PixelShaderInputType.hlsli"
#include "PixelShaderOutputType.hlsli"

PixelOutput PS(PixelShaderInput_PCNT input)
{
	PixelOutput output = (PixelOutput)0;

	output.Position = float4(input.World, 1);
	output.Normal = input.Normal;
	output.Color = Diffuse.Sample(Sampler, input.Diffuse);

	return output;
}
