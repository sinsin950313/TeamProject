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
	float4 p : SV_POSITION;
	float4 w : TEXCOORD3;
	float4 n : NORMAL0;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
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

	output.p = vProj;
	output.w = vWorld;
	output.c = input.color;
	output.n = mul(float4(input.normal.xyz, 0), input.worldInstance);
	output.t = input.tex;

	return output;
}
