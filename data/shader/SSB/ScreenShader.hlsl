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
SamplerComparisonState PCFSampler : register(s1);

cbuffer constant : register(b11)
{
	float4 currentCameraPos;
	float linearFogStart;
	float linearFogEnd;
	float expFogDensity;
};

#include "LightBufferData.hlsli"
#include "CameraBuffer.hlsli"

float GetShadowRatio(float2 uv)
{
	float4 pos = PositionMap.Sample(Sampler, uv);
	float4 vertexInLightCoordinate = mul(pos, LightViewMatrix);
	vertexInLightCoordinate = mul(vertexInLightCoordinate, LightProjMatrix);
	float vertexDistanceInLightCoordinate = vertexInLightCoordinate.z / vertexInLightCoordinate.w;
	float2 depthMapUV = vertexInLightCoordinate.xy / vertexInLightCoordinate.w;
	depthMapUV.x =  depthMapUV.x * 0.5 + 0.5;
	depthMapUV.y = -depthMapUV.y * 0.5 + 0.5;
	float depthInLightCoordinate = LightShadowMap.Sample(Sampler, depthMapUV).r;

	const float	g_iNumKernel = 3;

	float fLightAmount=0.0f;

	const float SMAP_SIZE = 2048 * 2;
	const float fdelta = 1.0f / SMAP_SIZE;
	int iHalf = (g_iNumKernel - 1) / 2;
	for (int v = -iHalf; v <= iHalf; v++)
	{
		for (int u = -iHalf; u <= iHalf; u++)
		{
			float2 vOffset = float2(u*fdelta, v*fdelta);	
			fLightAmount += LightShadowMap.SampleCmpLevelZero(PCFSampler, depthMapUV.xy+vOffset, vertexDistanceInLightCoordinate).r;
		}
	}
	fLightAmount /= g_iNumKernel*g_iNumKernel;	
	return fLightAmount;
}

float4 GetDiffuse(float2 uv)
{
	float4 normal = float4(NormalMap.Sample(Sampler, uv).xyz, 0);
	float4 lightDirection = LightWorldMatrix._31_32_33_34;
	float intensity = max(0, dot(normal, -lightDirection));
	float Attenuation = 1;

	float4 ret = LightColor * intensity * Attenuation;

	return ret;
}

float4 GetSpecular(float2 uv)
{
	float4 normal = float4(NormalMap.Sample(Sampler, uv).xyz, 0);
	float4 lightDirection = LightWorldMatrix._31_32_33_34;
	float4 reflectVector = reflect(lightDirection, normal);

	float4 eyePosition = CameraPosition._14_24_34_44;
	float4 eyeDirection = normalize(eyePosition - float4(PositionMap.Sample(Sampler, uv).xyz ,1));

	float powVal = 10;
	float Attenuation = 1;
	float intensity = pow(saturate(dot(eyeDirection, reflectVector)), powVal);

	float4 ret = LightColor * intensity * Attenuation;

	return ret;
}

float4 GetAmbient()
{
	return float4(0.2, 0.2, 0.2, 1);
}
bool IsBoundaryDepthNomral(float2 uv)
{
	int	g_iTexSizeX = 1600;
	int	g_iTexSizeY = 900;
	// -1 0 1
	// -2 0 2
	// -1 0 1
	float nx = 1.0f / g_iTexSizeX;
	float ny = 1.0f / g_iTexSizeY;
	int dx[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	int dy[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
	float depth = DepthMap.Sample(Sampler, uv).x;
	float diff = 0.0f;
	float3 normal = normalize(NormalMap.Sample(Sampler, uv).xyz);
	/*float3 viewVector = normalize(PositionMap.Sample(Sampler, uv).xyz - currentCameraPos.xyz);
	float cosTheta = dot(normal.xyz, -viewVector);*/
	float sum = 0.0f;
	for (int idx = 0; idx < 9; idx++)
	{
		float depthCheck = DepthMap.Sample(Sampler, float2(uv.x + dx[idx] * nx, uv.y + dy[idx] * ny)).x;
		diff += abs(depthCheck - depth);

		/*float3 normalCheck = normalize(NormalMap.Sample(Sampler, float2(uv.x + dx[idx] * nx, uv.y + dy[idx] * ny)).xyz);
		sum += saturate(dot(normalCheck, normal) - 0.2f);*/
	}
	
	if ( diff > 0.003f)
		return true;
	else
		return false;
}

bool IsBoundary(float2 uv)
{
	return false;
	float depth = DepthMap.Sample(Sampler, uv).x;
	depth = depth - 0.998;
	depth = depth / 0.002f;
	if(0.8f < depth)
	{
		return false;
	}

	float textureColor = DepthMap.Sample(Sampler, uv).x;
	int		g_iTexSizeX = 1600;
	int		g_iTexSizeY = 900;

	// -1 0 1
	// -2 0 2
	// -1 0 1
	float dx = 1.0f/g_iTexSizeX;
	float dy = 1.0f/g_iTexSizeY;

	float tl = DepthMap.Sample(Sampler, float2(uv.x - dx, uv.y - dy)).x;
	float l = DepthMap.Sample(Sampler, float2(uv.x - dx, uv.y)).x;
	float bl = DepthMap.Sample(Sampler, float2(uv.x - dx, uv.y + dy)).x;
	float t = DepthMap.Sample(Sampler, float2(uv.x, uv.y - dy)).x;
	float b = DepthMap.Sample(Sampler, float2(uv.x, uv.y + dy)).x;
	float tr = DepthMap.Sample(Sampler, float2(uv.x + dx, uv.y - dy)).x;
	float r = DepthMap.Sample(Sampler, float2(uv.x + dx, uv.y)).x;
	float br = DepthMap.Sample(Sampler, float2(uv.x + dx, uv.y + dy)).x;
	
	float SobelX = -tl - 2.0f * l - bl + tr + 2.0f * r + br;

	// -1 -2 -1
	// 0 0 0
	// 1 2 1
	float SobelY = -tl - 2.0f * t - tr + bl + 2.0f * b + br;

	//float3 N = normalize(float3(-SobelX.x, -SobelY.x, 1.0f));
	//N = N *0.5f +0.5f;

	float SobelResult = (abs(SobelX) + abs(SobelY)) / 2.0f;
	SobelResult = SobelResult / 0.002f;
	SobelResult = SobelResult - 0.998f;
		
	if (0.7f < SobelResult)
	{
		return false;
	}
	else 
	{
		return true;
	}	
}

float4 PS(PSInput input) : SV_TARGET0
{
	/*if(IsBoundary(input.TextureUV))
	{
		return float4(0, 0, 0, 1);
	}*/
	if (IsBoundaryDepthNomral(input.TextureUV))
	{
		return float4(0, 0, 0, 1);
	}

	// 선형 Fog 계산
	float4 pos = PositionMap.Sample(Sampler, input.TextureUV);
	float fogDist = distance(currentCameraPos, pos);
	float linearFogAmount = saturate((fogDist - linearFogStart) / (linearFogEnd - linearFogStart));

	// 지수 Fog 계산
	float expFogAmount = exp(-fogDist * expFogDensity);

	//fog
	float4 fogColor = float4(0.5f, 0.5f, 0.5f, 0.2f);

	// 선형 Fog와 지수 Fog의 양을 결합하여 최종 Fog 양을 계산
	float fogAmount = lerp(linearFogAmount, expFogAmount, 0.0f);

	float4 diffuseColor = ColorMap.Sample(Sampler, input.TextureUV);

	float4 ret = diffuseColor * GetAmbient();
	ret += diffuseColor * GetShadowRatio(input.TextureUV) * (GetDiffuse(input.TextureUV) + GetSpecular(input.TextureUV));
	// 개체의 색상에 Fog를 적용
	ret = lerp(ret, fogColor, fogAmount);
	return float4(ret.xyz, 1);
}

