#include "ConstantHeader.txt"
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
	float4 c : COLOR0;
	float2 t : TEXCOORD0;

	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;
	float2 texCoord4 : TEXCOORD4;
	float2 texCoord5 : TEXCOORD5;
	float2 texCoord6 : TEXCOORD6;
	float2 texCoord7 : TEXCOORD7;
	float2 texCoord8 : TEXCOORD8;
	float2 texCoord9 : TEXCOORD9;
};

VS_out VS(VS_in input)
{
	VS_out output = (VS_out)0;
	output.p = float4(input.p, 1);
	output.c = input.c;
	output.t = input.t;
	return output;
}

float4 PS(VS_out input) : SV_Target
{
	return g_txTexA.Sample(g_SampleWrap, input.t);
}

VS_out H_VS(VS_in input)
{
	VS_out output = (VS_out)0;

	output.p = float4(input.p, 1);
	output.c = input.c;
	output.t = input.t;
	g_vTemp.x;

	// 이 특정 너비의 화면에 대한 텍셀의 부동 소수점 크기를 결정합니다.
	float texelSize = 1.0f / g_vTemp.x;

	// 픽셀과 그 옆에 네 개의 수평 이웃에 대한 UV 좌표를 만듭니다.
	output.texCoord1 = saturate(input.t + float2(texelSize * -4.0f, 0.0f));
	output.texCoord2 = saturate(input.t + float2(texelSize * -3.0f, 0.0f));
	output.texCoord3 = saturate(input.t + float2(texelSize * -2.0f, 0.0f));
	output.texCoord4 = saturate(input.t + float2(texelSize * -1.0f, 0.0f));
	output.texCoord5 = saturate(input.t + float2(texelSize * 0.0f, 0.0f));
	output.texCoord6 = saturate(input.t + float2(texelSize * 1.0f, 0.0f));
	output.texCoord7 = saturate(input.t + float2(texelSize * 2.0f, 0.0f));
	output.texCoord8 = saturate(input.t + float2(texelSize * 3.0f, 0.0f));
	output.texCoord9 = saturate(input.t + float2(texelSize * 4.0f, 0.0f));

	return output;
}

float4 H_PS(VS_out input) : SV_Target
{
	float weight0, weight1, weight2, weight3, weight4;
	float normalization;
	float4 color;


	// 각 이웃 픽셀이 흐림에 기여할 가중치를 만듭니다.
	weight0 = 1.0f;
	weight1 = 0.9f;
	weight2 = 0.55f;
	weight3 = 0.18f;
	weight4 = 0.1f;

	// 가중치를 평균화하는 정규화 된 값을 만듭니다.
	normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4));

	// 가중치를 표준화합니다.
	weight0 = weight0 / normalization;
	weight1 = weight1 / normalization;
	weight2 = weight2 / normalization;
	weight3 = weight3 / normalization;
	weight4 = weight4 / normalization;

	// 검은 색으로 초기화합니다.
	color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 각각의 특정 가중치로 9 개의 수직 픽셀을 색상에 추가합니다.
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord1) * weight4;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord2) * weight3;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord3) * weight2;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord4) * weight1;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord5) * weight0;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord6) * weight1;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord7) * weight2;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord8) * weight3;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord9) * weight4;

	return color;
}


VS_out V_VS(VS_in input)
{
	VS_out output = (VS_out)0;

	output.p = float4(input.p, 1);
	output.c = input.c;
	output.t = input.t;

	// 이 특정 너비의 화면에 대한 텍셀의 부동 소수점 크기를 결정합니다.
	float texelSize = 1.0f / g_vTemp.y;

	// 픽셀과 그 옆에 네 개의 수평 이웃에 대한 UV 좌표를 만듭니다.
	output.texCoord1 = saturate(input.t + float2(0.0f, texelSize * -4.0f));
	output.texCoord2 = saturate(input.t + float2(0.0f, texelSize * -3.0f));
	output.texCoord3 = saturate(input.t + float2(0.0f, texelSize * -2.0f));
	output.texCoord4 = saturate(input.t + float2(0.0f, texelSize * -1.0f));
	output.texCoord5 = saturate(input.t + float2(0.0f, texelSize * 0.0f));
	output.texCoord6 = saturate(input.t + float2(0.0f, texelSize * 1.0f));
	output.texCoord7 = saturate(input.t + float2(0.0f, texelSize * 2.0f));
	output.texCoord8 = saturate(input.t + float2(0.0f, texelSize * 3.0f));
	output.texCoord9 = saturate(input.t + float2(0.0f, texelSize * 4.0f));

	return output;
}

float4 V_PS(VS_out input) : SV_TARGET
{
	float weight0, weight1, weight2, weight3, weight4;
	float normalization;
	float4 color;


	// 각 이웃 픽셀이 흐림에 기여할 가중치를 만듭니다.
	weight0 = 1.0f;
	weight1 = 0.9f;
	weight2 = 0.55f;
	weight3 = 0.18f;
	weight4 = 0.1f;

	// 가중치를 평균화하는 정규화 된 값을 만듭니다.
	normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4));

	// 가중치를 표준화합니다.
	weight0 = weight0 / normalization;
	weight1 = weight1 / normalization;
	weight2 = weight2 / normalization;
	weight3 = weight3 / normalization;
	weight4 = weight4 / normalization;

	// 검은 색으로 초기화합니다.
	color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 각각의 특정 가중치로 9 개의 수직 픽셀을 색상에 추가합니다.
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord1) * weight4;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord2) * weight3;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord3) * weight2;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord4) * weight1;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord5) * weight0;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord6) * weight1;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord7) * weight2;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord8) * weight3;
	color += g_txTexA.Sample(g_SampleWrap, input.texCoord9) * weight4;

	return color;
}