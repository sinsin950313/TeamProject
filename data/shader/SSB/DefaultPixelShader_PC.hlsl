#include "PixelShaderInputType.hlsli"
#include "PixelShaderOutputType.hlsli"

PixelOutput PS(PixelShaderInput_PC input)
{
	PixelOutput output = (PixelOutput)0;

	output.Position = input.Position;
	output.Normal = float4(0, 0, 1, 0);
	output.Color = input.Color;

	return output;
}
