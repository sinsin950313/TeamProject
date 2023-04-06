#include "SSB/VertexShaderInputType.hlsli"
#include "SSB/SSBConstantBuffer.hlsli"
#include "SSB/VertexShaderOutputType.hlsli"

struct Vertex_PCNT_Inst
{
	float4 Position : Position;
	float4 Color : Color;
	float4 Normal : Normal;
	float2 Diffuse : Diffuse;
	int4 BoneIndex : AffectingBoneIndex;
	float4 Weight : Weight;
	uint  iVID : SV_InstanceID;
};

cbuffer InstancingData : register(b8)
{
	matrix g_mInstancing[12];
	float4 g_vInstColor[12];
};

VertexOutput_PCNT VS(Vertex_PCNT_Inst input)
{
	VertexOutput_PCNT output = (VertexOutput_PCNT)0;

	float4 pos = 0;
	float4 normal = 0;

	{
		pos = mul(input.Position, MeshAnimationMatrix[SkinningMeshIndex]) * SkinningMeshWeight;
		normal = mul(input.Normal, MeshAnimationMatrix[SkinningMeshIndex]) * SkinningMeshWeight;
	}
	
	for (int i = 0; i < 4; ++i)
	{
		int index = input.BoneIndex[i];
		float weight = input.Weight[i];
		matrix toBoneSpaceMatrix = ToBoneSpaceTransformMatrix[index];
		matrix boneAnimationMatrix = BoneAnimationMatrix[index];

		float4 tmpPos = mul(input.Position, toBoneSpaceMatrix);
		pos += mul(tmpPos, boneAnimationMatrix) * weight;

		float4 tmpNormal = mul(input.Normal, toBoneSpaceMatrix);
		normal += mul(tmpNormal, boneAnimationMatrix) * weight;
	}

	float4 world = mul(pos, g_mInstancing[input.iVID]);
	float4 view = mul(world, ViewMatrix);
	float4 proj = mul(view, ProjectionMatrix);

	output.p = proj;
	output.n = normal;
	output.c = g_vInstColor[input.iVID];
	output.t = input.Diffuse;

	return output;
}
