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
// Need Light Direction Map
// Need Material Power Value Map

SamplerState Sampler : register(s0);

#include "LightBufferData.hlsli"
#include "CameraBuffer.hlsli"

float4 GetDiffuse(float2 uv)
{
	float4 normal = float4(NormalMap.Sample(Sampler, uv).xyz, 0);
	float4 lightDirection = LightWorldMatrix._13_23_33_43;
	float intensity = dot(normal, -lightDirection);
	float Attenuation = 1;

	float4 ret = LightColor * intensity * Attenuation;

	return ret;
}

float4 GetSpecular(float2 uv)
{
	float4 normal = float4(NormalMap.Sample(Sampler, uv).xyz, 0);
	float4 lightDirection = LightWorldMatrix._13_23_33_43;
	float4 reflectVector = reflect(lightDirection, normal);
	float powVal = 1;
	float Attenuation = 1;
	float4 eyePosition = CameraPosition._41_42_43_44;
	float4 eyeDirection = normalize(eyePosition - float4(PositionMap.Sample(Sampler, uv).xyz ,1));
	eyeDirection = float4(0, 0, -1, 0);

	float intensity = min(1, max(0, pow(dot(eyeDirection, reflectVector), powVal)));
	float4 ret = LightColor * intensity * Attenuation;

	return ret;
}

float4 GetAmbient()
{
	return float4(0.3, 0.3, 0.3, 1);
}

float4 PS(PSInput input) : SV_TARGET0
{
	float4 diffuseColor = ColorMap.Sample(Sampler, input.TextureUV);

	float4 ret = diffuseColor * (GetDiffuse(input.TextureUV) + GetSpecular(input.TextureUV) + GetAmbient());

	return float4(ret.xyz, 1);
}

