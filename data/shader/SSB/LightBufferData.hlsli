cbuffer AmbientLightData : register(b10)
{
	float3 LightAmbientCoefficient;
}

cbuffer LightData : register(b9)
{
	float4 LightColor;
};

cbuffer DirectionalLightData : register(b8)
{
};

cbuffer PointLightData : register(b7)
{
	float LightRadius;
}

cbuffer SpotLightData : register(b6)
{
	float LightInCornRadius;
	float LightOutCornRadius;
}

struct LightData
{
	float4 Position;
	float4 Direction;
	float4 Color;
}

struct LightComputationParam
{
	float4 EyeDirection;
	float SpecularFactor;
	float3 Padding;
}
