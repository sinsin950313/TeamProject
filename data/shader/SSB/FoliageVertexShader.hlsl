struct VS_INPUT
{
	float4 position : Position;
	float4 color : Color;
	float4 normal : Normal;
	float2 tex : Diffuse;

	row_major float4x4 worldInstance : WORLD;
};

struct VS_OUTPUT
{
	float4 projection : SV_POSITION;
	float4 world : TEXCOORD3;
	float4 normal : NORMAL0;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
};

cbuffer Camera : register(b0)
{
	matrix CameraPosition : packoffset(c0);
	matrix ViewMatrix : packoffset(c4);
	matrix ProjectionMatrix : packoffset(c8);
};

cbuffer Object : register(b1)
{
	matrix WorldTransformMatrix : packoffset(c0);
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	input.position.w = 1.0f;
	float4 vWorld = mul(input.position, input.worldInstance);
	float4 vView = mul(vWorld, ViewMatrix);
	float4 vProj = mul(vView, ProjectionMatrix);

	output.projection = vProj;
	output.world = vWorld;
	output.normal = float4(normalize(mul(input.normal, input.worldInstance)).xyz, 1.0f);
	output.tex = input.tex;
	output.color = input.color;

	return output;
}
