Texture2D TextureColor : register(t0);
Texture2D g_txMaskTex : register(t1);
Texture2D g_txTex2 : register(t2);
Texture2D g_txTex3 : register(t3);
Texture2D g_txTex4 : register(t4);
Texture2D g_txTex5 : register(t5);

sampler TextureSamplerColor : register(s0);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 world : TEXCOORD3;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 color : COLOR0;
	float3 direction_to_camera : TEXCOORD1;
	float4 m_light_direction : TEXCOORD2;
	float4 tex2 : TEXCOORD4;
	//float linearFogAmount : LINEAR_FOG_AMOUNT;
	//float expFogAmount : EXP_FOG_AMOUNT;
};

//if using row_major, not transpose in cpp
cbuffer constant : register(b0)
{
	row_major float4x4 matWorld;
	row_major float4x4 matView;
	row_major float4x4 matProj;
};

cbuffer constant : register(b1)
{
	float2 worldSize;
	float cellDistance;
	int tileCount;
};

cbuffer constant : register(b2)
{
	float4 lightDirection;
	float4 cameraPosition;
};

struct MRTOutput
{
	float4 Position : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Color : SV_TARGET2;
};

float4 GetTextureColor(float2 uv)
{
	float4 texelColor = TextureColor.Sample(TextureSamplerColor, uv * tileCount);
	float4 mask = g_txMaskTex.Sample(TextureSamplerColor, uv);
	float4 splatTex2 = g_txTex2.Sample(TextureSamplerColor, uv * tileCount);
	float4 splatTex3 = g_txTex3.Sample(TextureSamplerColor, uv * tileCount);
	float4 splatTex4 = g_txTex4.Sample(TextureSamplerColor, uv * tileCount);
	float4 splatTex5 = g_txTex5.Sample(TextureSamplerColor, uv * tileCount);

	// Calculate weights based on mask
	float4 weights = mask.rgba;

	// Calculate final color based on weights and splat textures

	float4 finalColor = texelColor * (1.0f - weights.r) + splatTex2 * weights.r;
	finalColor = finalColor * (1.0f - weights.g) + splatTex3 * weights.g;
	finalColor = finalColor * (1.0f - weights.b) + splatTex4 * weights.b;
	finalColor = finalColor * (1.0f - weights.a) + splatTex5 * weights.a;

	return finalColor;
}

MRTOutput psmain(PS_INPUT input) : SV_TARGET
{
	MRTOutput output = (MRTOutput)0;

	output.Position = float4(input.world, 1);
	output.Normal = float4(input.normal, 1);
	////fog
	//float4 fogColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

	//// 선형 Fog와 지수 Fog의 양을 결합하여 최종 Fog 양을 계산
	//float fogAmount = lerp(input.linearFogAmount, input.expFogAmount, 0.0f);

	//// 개체의 색상에 Fog를 적용
	//fogColor.rgb = lerp(GetTextureColor(input.tex).rgb, fogColor, fogAmount);

	output.Color = GetTextureColor(input.tex);

	return output;

	//AmbientLight
	float4 finalColor = GetTextureColor(input.tex);
	float ka = 0.1f;
	float3 ia = float3(1.0f, 1.0f, 1.0f);
	ia *= finalColor.rgb;
	float3 ambient_light = ka * ia;

	//DiffuseLight
	float kd = 0.8f;
	float3 id = float3(1.0f, 1.0f, 1.0f);
	id *= finalColor.rgb;

	float amount_diffuse_light = max(0.0f, dot(input.normal , -input.m_light_direction.xyz));
	float3 diffuse_light = kd * amount_diffuse_light * id;

	//SpecularLight
	float ks = 1.0f;
	float3 is = float3(1.0f, 1.0f, 1.0f);
	is *= finalColor.rgb;
	float3 reflected_light = reflect(input.m_light_direction.xyz, input.normal);
	float shininess = 10.0f;
	float amount_specular_light = pow(max(0.0f, dot(reflected_light, input.direction_to_camera)), shininess);

	float3 specular_light = ks * amount_specular_light * is;

	float3 final_light = ambient_light + diffuse_light + specular_light;

	//return float4(final_light.rgb, 1.0f);
}
