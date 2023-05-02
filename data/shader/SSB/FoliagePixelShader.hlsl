Texture2D TextureColor : register(t0);
sampler TextureSamplerColor : register(s0);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL0;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
};

struct PixelOutput
{
	float4 Position : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Color : SV_TARGET2;
};

PixelOutput PS(PS_INPUT input)
{
	PixelOutput output = (PixelOutput)0;
	output.Position = input.position;
	output.Normal = input.normal;
	output.Color = TextureColor.Sample(TextureSamplerColor, input.tex);
	if (output.Color.w < 0.1 || output.Color.r < 0.1)
		discard;
	return output;
}