#include "VertexShaderInputType.hlsli"
#include "SSBConstantBuffer.hlsli"
#include "VertexShaderOutputType.hlsli"

VertexOutput_PCNT VS(Vertex_PCNT_Skinning input)
{
	VertexOutput_PCNT output = (VertexOutput_PCNT)0;

	float4 localNormal = float4(input.Normal.xyz, 0);

	float4 pos = 0;
	float4 normal = 0;

	{
		pos = mul(input.Position, MeshAnimationMatrix[SkinningMeshIndex]) * SkinningMeshWeight;
		normal = mul(localNormal, MeshAnimationMatrix[SkinningMeshIndex]) * SkinningMeshWeight;
	}
	
	for (int i = 0; i < 4; ++i)
	{
		int index = input.BoneIndex[i];
		float weight = input.Weight[i];
		matrix toBoneSpaceMatrix = ToBoneSpaceTransformMatrix[index];
		matrix boneAnimationMatrix = BoneAnimationMatrix[index];

		float4 tmpPos = mul(input.Position, toBoneSpaceMatrix);
		pos += mul(tmpPos, boneAnimationMatrix) * weight;

		float4 tmpNormal = mul(localNormal, toBoneSpaceMatrix);
		normal += mul(tmpNormal, boneAnimationMatrix) * weight;
	}

	float4 world = mul(pos, WorldTransformMatrix);
	float4 view = mul(world, ViewMatrix);
	float4 proj = mul(view, ProjectionMatrix);

	float4 worldNormal = float4(mul(normal, WorldTransformMatrix).xyz, 1);

	output.p = proj;
	output.n = input.Normal;
	output.c = input.Color;
	output.t = input.Diffuse;

	return output;
}
