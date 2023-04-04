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
Texture2D LightShadowMap : register(t4);

SamplerState Sampler : register(s0);

#include "LightBufferData.hlsli"

float4 GetDiffuse(float2 uv)
{
	float4 normal = float4(NormalMap.Sample(Sampler, uv).xyz, 0);
	float4 lightDirection = LightWorldMatrix._13_23_33_43;
	float intensity = min(1, max(0, dot(normal, -lightDirection)));

	float4 ret = LightColor * intensity * 1;//* Attenuation;

	return ret;
}

float4 PS(PSInput input) : SV_TARGET0
{
	float4 diffuseColor = ColorMap.Sample(Sampler, input.TextureUV);

	float4 ret = diffuseColor * (GetDiffuse(input.TextureUV) + float4(0.3, 0.3, 0.3, 1));

	return float4(ret.xyz, 1);
}

