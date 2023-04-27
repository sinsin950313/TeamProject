#include "ConstantHeader.txt"
Texture2D	g_ColorTex		: register(t9);
struct VS_in
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXCOORD;
};
struct VS_out
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

VS_out VS(VS_in input)
{
	VS_out output = (VS_out)0;

	output.p = float4(input.p, 1.0f);
	output.p = mul(output.p, g_matWorld);
	output.p = mul(output.p, g_matView);
	output.p = mul(output.p, g_matProj);
	output.n = input.n;
	output.c = input.c;
	output.t = input.t;
	return output;
}

float4 PS_Blur(VS_out input) : SV_Target
{
	float weights[9] = {0.0325, 0.07, 0.0325, 0.07, 0.1, 0.07, 0.0325, 0.07, 0.0325};
	float2 texelSize = float2(1.0f / 1600.0f, 1.0f / 900.0f);

	float4 color = float4(0, 0, 0, 0);
	//float4 vColor = g_ColorTex.Sample(g_SampleWrap , input.t);
	float4 alpha = g_txTexA.Sample(g_SampleWrap, input.t);
	for (int i = -4; i <= 4; ++i)
	{
		float2 offset = float2(texelSize.x * i, texelSize.y * i);
		color += g_ColorTex.Sample(g_SampleWrap, input.t + offset) * weights[i + 4];
	}


	return float4(color.rgb, alpha.a * input.c.w);
}
//float4 PS_Vertical(VS_out input) : SV_Target
//{
//	float weights[9] = {0.0625, 0.125, 0.0625, 0.125, 0.25, 0.125, 0.0625, 0.125, 0.0625};
//	float2 texelSize = float2(1.0f / 1600.0f, 1.0f / 900.0f);
//
//	float4 color = float4(0, 0, 0, 0);
//	//float4 vColor = g_ColorTex.Sample(g_SampleWrap , input.t);
//	float4 alpha = g_txTexA.Sample(g_SampleWrap, input.t);
//	for (int i = -4; i <= 4; ++i)
//	{
//		float2 offset = float2(0, texelSize.y * i);
//		color += g_ColorTex.Sample(g_SampleWrap, input.t + offset) * weights[i + 4];
//	}
//
//	/*if (vColor.a < 0.1f)
//		discard;
//	return vColor * input.c;*/
//
//	return float4(color.rgb, alpha.a);
//}