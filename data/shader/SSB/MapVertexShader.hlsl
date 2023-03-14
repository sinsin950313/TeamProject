struct VS_INPUT
{
	float4 position : POSITION0;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 color : COLOR0;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 color : COLOR0;
	float3 direction_to_camera : TEXCOORD1;
	float4 m_light_direction : TEXCOORD2;
	float3 world : TEXCOORD3;
	float4 tex2 : TEXCOORD4;
};

//if using row_major, not transpose in cpp
cbuffer constant : register(b0)
{
	row_major float4x4 matWorld;
	row_major float4x4 matView;
	row_major float4x4 matProj;
	float4 m_light_direction;
	float4 m_camera_position;
	float2 m_world_size;
	float cell_distance;
};

VS_OUTPUT vsmain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 vLocal = input.position;
	float4 vWorld = mul(vLocal, matWorld);
	float4 vView = mul(vWorld, matView);
	float4 vProj = mul(vView, matProj);
	//output.direction_to_camera = normalize(output.position.xyz - m_camera_position.xyz); //dlight
	//output.direction_to_camera = normalize(m_camera_position.xyz - output.position.xyz); //plight
	output.direction_to_camera = normalize(vWorld.xyz - m_camera_position.xyz);
	output.m_light_direction = float4(output.direction_to_camera, 1.0f);


	output.world = vWorld.xyz;

	float tileCnt = 1.0f;
	int world_half = m_world_size.x / 2;
	output.tex2.x = (vWorld.x + world_half * cell_distance) / m_world_size.x; //0 ~ 1
	output.tex2.x = output.tex2.x * tileCnt;
	output.tex2.y = 1.0f - ((vWorld.z / world_half * cell_distance) / 2.0f + 0.5f); //0 ~ 1
	output.tex2.y = output.tex2.y * tileCnt;
	// 텍스처 변환 행렬 사용 world to tex
	matrix matTex = 0;
	matTex._11 = 1.0f / ((m_world_size.x / tileCnt) * cell_distance);
	matTex._32 = -1.0f / ((m_world_size.x / tileCnt) * cell_distance);
	matTex._41 = 0.0f; // 타일에 개수 홀수=0.5f, 짝수=0.0f
	matTex._42 = 0.0f;
	float4 vUV = mul(vWorld, matTex);
	output.tex2.x = vUV.x;
	output.tex2.y = vUV.y;

	// 투영좌표 사용
	output.tex2.z = (vProj.x / vProj.w) * 0.5f + 0.5f;
	output.tex2.w = (vProj.y / vProj.w) * 0.5f + 0.5f;

	output.position = vProj;
	output.normal = input.normal;
	output.tex = input.tex;
	output.color = input.color;
	return output;
}