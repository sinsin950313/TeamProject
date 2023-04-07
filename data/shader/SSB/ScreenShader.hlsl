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

bool IsInShadow(float2 uv)
{
	////const float	g_iNumKernel = 3;
	//float4 vDiffuseColor = g_txDiffuse.Sample( g_samLinear, input.t );
	//float fLightAmount=0.0f;
	//float3 ShadowTexColor =input.TexShadow.xyz / input.TexShadow.w;

	//const float fdelta = 1.0f / SMAP_SIZE;
	//int iHalf = (g_iNumKernel - 1) / 2;
	//for (int v = -iHalf; v <= iHalf; v++)
	//{
	//	for (int u = -iHalf; u <= iHalf; u++)
	//	{
	//		float2 vOffset = float2(u*fdelta, v*fdelta);	
	//		fLightAmount += g_txDepthMap.SampleCmpLevelZero(g_samComShadowMap, 
	//								ShadowTexColor.xy+vOffset, ShadowTexColor.z).r;
	//	}											
	//}		
	//fLightAmount /= g_iNumKernel*g_iNumKernel;	
	//float fColor = fLightAmount;
	//float4 vFinalColor = vDiffuseColor*max(0.5f, fLightAmount);
	//vFinalColor.a = 1.0f;
	//return  vFinalColor;
	
	return false;
}

float4 GetDiffuse(float2 uv)
{
	float4 normal = float4(NormalMap.Sample(Sampler, uv).xyz, 0);
	float4 lightDirection = LightWorldMatrix._13_23_33_43;
	float intensity = max(0, dot(normal, -lightDirection));
	float Attenuation = 1;

	float4 ret = LightColor * intensity * Attenuation;

	return ret;
}

float4 GetSpecular(float2 uv)
{
	float4 normal = float4(NormalMap.Sample(Sampler, uv).xyz, 0);
	float4 lightDirection = LightWorldMatrix._13_23_33_43;
	float4 reflectVector = reflect(lightDirection, normal);

	float4 eyePosition = CameraPosition._14_24_34_44;
	float4 eyeDirection = normalize(eyePosition - float4(PositionMap.Sample(Sampler, uv).xyz ,1));

	float powVal = 10;
	float Attenuation = 1;
	float intensity = min(1, max(0, pow(dot(eyeDirection, reflectVector), powVal)));

	float4 ret = LightColor * intensity * Attenuation;

	return ret;
}

float4 GetAmbient()
{
	return float4(0.4, 0.4, 0.4, 1);
}

float4 PS(PSInput input) : SV_TARGET0
{
	float4 diffuseColor = ColorMap.Sample(Sampler, input.TextureUV);

	float4 ret = diffuseColor * GetAmbient();
	if(IsInShadow(input.TextureUV))
	{
		ret += diffuseColor * (GetDiffuse(input.TextureUV) + GetSpecular(input.TextureUV));
	}

	return float4(ret.xyz, 1);
}

