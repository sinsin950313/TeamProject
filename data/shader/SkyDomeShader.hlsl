#include "ConstantHeader.txt"

struct VS_in
{
	float4 p : POSITION0;
	float3 n : NORMAL0;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};
struct VS_out
{
	float4 p : SV_POSITION;
	float4 w : TEXCOORD3;
	float4 c : COLOR0;
	float4 n : NORMAL0;
	float2 t : TEXCOORD0;
};

VS_out VS(VS_in input)
{
	VS_out output = (VS_out)0;

	output.p = mul(input.p, g_matWorld);
	output.p = mul(output.p, g_matView);
	output.p = mul(output.p, g_matProj);
	output.w = mul(input.p, g_matWorld);
	output.n = float4(input.n, 0);
	output.c = input.c;
	output.t = input.t;
	return output;
}

struct PS_in
{
	float4 p : SV_POSITION;
	float4 w : TEXCOORD3;
	float4 c : COLOR0;
	float4 n : NORMAL0;
	float2 t : TEXCOORD0;
};

struct PixelOutput
{
	float4 Position : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Color : SV_TARGET2;
};

PixelOutput PS(PS_in input) : SV_Target
{
	PixelOutput output = (PixelOutput)0;

	output.Position = input.w;
	output.Normal = input.n;
	output.Color = g_txTexA.Sample(g_SampleWrap , input.t);

	return output;
}

float4 COLOR_PS(VS_out input) : SV_Target
{
	return input.c;
}
