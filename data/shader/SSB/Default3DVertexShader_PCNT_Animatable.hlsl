#include "VertexShaderInputType.hlsli"
#include "SSBConstantBuffer.hlsli"
#include "VertexShaderOutputType.hlsli"

VertexOutput_PCNT VS(Vertex_PCNT input)
{
	VertexOutput_PCNT output = (VertexOutput_PCNT)0;

	float4 localNormal = float4(input.Normal.xyz, 0);

	float4 pos = 0;
	float4 normal = 0;
	{
		pos = mul(input.Position, MeshAnimationMatrix[MeshIndex]);
		normal = mul(localNormal, MeshAnimationMatrix[MeshIndex]);
	}

	float4 world = mul(pos, WorldTransformMatrix);
	float4 view = mul(world, ViewMatrix);
	float4 proj = mul(view, ProjectionMatrix);

	float4 worldNormal = float4(normalize(mul(normal, WorldTransformMatrix)).xyz, 1);

	output.p = proj;
	output.w = world;
	output.n = worldNormal;
	output.c = input.Color;
	output.t = input.Diffuse;

	return output;

}
