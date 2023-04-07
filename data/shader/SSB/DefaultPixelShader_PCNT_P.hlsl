#include "PixelShaderInputType.hlsli"
#include "PixelShaderOutputType.hlsli"

PixelOutput PS(PixelShaderInput_PCNT_P input) : SV_TARGET
{
	PixelOutput output = (PixelOutput)0;

	output.Position = float4(input.Position, 1);
	output.Normal = input.Normal;
	output.Color = float4(GetDiffuseColor(input.Index, input.Diffuse).xyz, 1);

	return output;
}
