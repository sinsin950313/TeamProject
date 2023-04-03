struct VSInput
{
	float3 Position : Position;
	float2 TextureUV : TextureUV;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float2 TextureUV : TEXCOORD0;
};

VSOutput VS(VSInput input)
{
	VSOutput output = (VSOutput)0;

	output.Position = float4(input.Position, 1);
	output.TextureUV = input.TextureUV;

	return output;
}

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 TextureUV : TEXCOORD0;
};

Texture2D PositionMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D ColorMap : register(t2);
Texture2D DepthMap : register(t3);

SamplerState Sampler : register(s0);

float4 PS(PSInput input) : SV_TARGET0
{
	return ColorMap.Sample(Sampler, input.TextureUV);
}

