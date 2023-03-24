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
	float3 n : NORMAL0;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

VS_out VS(VS_in input)
{
	VS_out output = (VS_out)0;
	output.p = mul(input.p, g_matWorld);
	output.p = mul(output.p, g_matView);
	output.p = mul(output.p, g_matProj);
	output.n = input.n;
	output.c = input.c;
	output.t = input.t;
	return output;
}

struct PS_in
{
	float4 p : SV_POSITION;
	float3 n : NORMAL0;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

float4 PS(PS_in input) : SV_Target
{
	float4 vColor = g_txTexA.Sample(g_SampleWrap , input.t);
	return vColor;
}

float4 COLOR_PS(VS_out input) : SV_Target
{
	return input.c;
}