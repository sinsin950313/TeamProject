#include "ConstantHeader.txt"
Texture2D	g_ColorTex		: register(t9);

struct VS_in
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXCOORD;
	uint iVID : SV_InstanceID;
};
struct VS_out
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

cbuffer InstancingData : register(b8)
{
	matrix g_mInstancing[200];
	float4 g_vInstColor[200];
	float4 g_vCalc[200][2];
	float4 g_vUV[200][2];
};

VS_out VS(VS_in input)
{
	VS_out output = (VS_out)0;

	float4 vLocal = float4(input.p, 1.0f);
	float4 vWorld = mul(vLocal, g_mInstancing[input.iVID]);
	float4 vView = mul(vWorld, g_matView);
	float4 vProj = mul(vView, g_matProj);

	output.p = vProj;
	output.n = input.n;
	output.c = input.c * g_vInstColor[input.iVID];
	output.t = input.t;

	return output;
}

struct PS_OUT
{
	float4 vColor :SV_TARGET0;//add(+)
	float4 vBlack :SV_TARGET1;//multiply(*)
};

PS_OUT PS(VS_out input) : SV_Target
{
	PS_OUT vOut;
	vOut.vColor = g_txTexA.Sample(g_SampleWrap, input.t);
	vOut.vColor.rgb = vOut.vColor.rgb * input.c.rgb * input.c.a;
	vOut.vColor.a = 1;

	vOut.vBlack = float4(1 - vOut.vColor.r, 1 - vOut.vColor.g, 1 - vOut.vColor.b, 1);
	return vOut;
}

float4 Distortion(VS_out input) : SV_Target
{
	float4 vOut;
	float4 color = g_txTexA.Sample(g_SampleWrap, input.t);
	float2 fWeight = color.rg * 0.5f - 0.5;
	fWeight = fWeight * 0.2;

	float2 xy; ((input.p.xy / 2048.0f) + 1) / 2;
	xy.x = input.p.x / 2048.0f;
	xy.y = input.p.y / 2048.0f;

	vOut = g_ColorTex.Sample(g_SampleWrap, xy + fWeight);
	float alpha = (0.5 - (abs(input.t.x - 0.5))) + (0.5 - (abs(input.t.y - 0.5)));
	vOut.a = alpha;
	if (color.a <= 0.1)
		discard;

	return vOut;
}

float4 COLOR_PS(VS_out input) : SV_Target
{
	return input.c;
}

float4 OPACITY_PS(VS_out input) : SV_Target
{
	float4 color = g_txTexA.Sample(g_SampleWrap, input.t);
	return color * input.c;
}


struct GS_in
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
	float3 w : TEXCOORD1;
	uint iVID : SV_InstanceID;
};
GS_in RibbonVS(VS_in input)
{
	GS_in output = (GS_in)0;

	output.p = float4(input.p, 1.0f);
	output.c = input.c;
	output.t = input.t;
	output.iVID = input.iVID;

	return output;
}

[maxvertexcount(3)]
void RibbonGS( triangle GS_in input[3], inout TriangleStream<VS_out> ribbonStream )
{	
	// 0 1
	// 2 3
	// 0 -> 1 -> 2
	// 2 -> 1 -> 3
	uint iVID = input[0].iVID;
	const float3 g_positions[4] =
	{
		float3(-1.0,  1.0, 0.0),
		float3(1.0,  1.0, 0.0),
		float3(-1.0, -1.0, 0.0),
		float3(1.0, -1.0, 0.0),
	};
	//float4 vPrev[4];
	//float4 vCur[4];
	//float4 vNext[4];
	//float4 vPos[4];
	
	float4 ribbonPos[4] =
	{
		g_vCalc[iVID][0],
		g_vCalc[iVID+1][0],
		g_vCalc[iVID][1],
		g_vCalc[iVID+1][1]
	};
	float4 ribbonUV[4] =
	{
		g_vUV[iVID][0],
		g_vUV[iVID + 1][0],
		g_vUV[iVID][1],
		g_vUV[iVID + 1][1]
	};

	int3 usingIndex = all(g_positions[0] == input[0].p.xyz) ? int3(0, 1, 2) : int3(2, 1, 3);
	for (int v = 0; v < 3; v++)
	{
		VS_out output;

		// nextID는 배열 개수로 프로그램 단계에서 처리를 미리 했음
		int curIndex = usingIndex[v];
		float4 vPos = ribbonPos[curIndex];
		float4 vView = mul(vPos, g_matView);
		float4 vProj = mul(vView, g_matProj);

		output.p = vProj;
		output.n = input[v].n;
		output.c = input[v].c * g_vInstColor[iVID];
		output.t = ribbonUV[curIndex].xy;

		ribbonStream.Append(output);
	}
	ribbonStream.RestartStrip();
}


cbuffer MaterialConstantBuffer : register(b7)
{
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
	float4 emissiveColor;
	float shininess;
	float padding[3];
};

/*
// Pixel Shader
float4 Emissive(VS_out input) : SV_Target
{

	// diffuse lighting을 계산합니다.
	float4 diffuse = texDiffuse.Sample(samLinear, input.tex);
	float3 lightDirection = normalize(float3(1.0f, 1.0f, 1.0f));
	float diffuseIntensity = max(dot(lightDirection, float3(0.0f, 0.0f, -1.0f)), 0.0f);
	float4 diffuseColor = diffuse * diffuseIntensity;

	// emissive 값을 반환합니다.
	float4 emissive = texDiffuse.Sample(samLinear, input.tex);
	return emissive + diffuseColor;

	////////////////////////////////////////////////
	float3 lightDirection = normalize(LightDirection.xyz);
	float3 viewDirection = normalize(EyeDirection.xyz);
	float3 reflectedLight = reflect(-lightDirection, normal);

	float4 ambient = ambientColor * AmbientIntensity;
	float4 diffuse = diffuseColor * saturate(dot(normal, lightDirection));
	float4 specular = specularColor * pow(saturate(dot(reflectedLight, viewDirection)), shininess);
	float4 emissive = emissiveColor; // 추가: emissive 값

	output = emissive + ambient + diffuse + specular;
}*/