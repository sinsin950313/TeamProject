cbuffer LightData : register(b10)
{
	matrix LightWorldMatrix;
	matrix LightViewMatrix;
	matrix LightProjMatrix;
	float4 LightColor;
};

cbuffer DirectionalLightData : register(b9)
{
};

cbuffer PointLightData : register(b8)
{
	float LightRadius;
};

cbuffer SpotLightData : register(b7)
{
	float LightInCornRadius;
	float LightOutCornRadius;
};

cbuffer AmbientLightData : register(b6)
{
	float3 LightAmbientCoefficient;
};

struct LightComputationParam
{
	float4 EyeDirection;
	float SpecularFactor;
	float3 Padding;
};
