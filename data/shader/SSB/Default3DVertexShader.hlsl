#include "VertexShaderInputType.hlsli"
#include "SSBConstantBuffer.hlsli"
#include "VertexShaderOutputType.hlsli"

VertexOutput_PC PC_VS(Vertex_PC input)
{
	float4 pos = input.Position;

	pos = mul(input.Position, WorldTransformMatrix);
	pos = mul(pos, ViewMatrix);
	pos = mul(pos, ProjectionMatrix);

	VertexOutput_PC output = (VertexOutput_PC)0;
	output.p = pos;
	output.c = input.Color;

	return output;
}

VertexOutput_PCNT PCNT_VS(Vertex_PCNT input)
{
	float4 pos = mul(input.Position, WorldTransformMatrix);
	pos = mul(pos, ViewMatrix);
	pos = mul(pos, ProjectionMatrix);

	VertexOutput_PCNT output = (VertexOutput_PCNT)0;
	output.p = pos;
	output.c = input.Color;
	output.n = input.Normal;
	output.t = input.Diffuse;

	return output;
}

VertexOutput_PCNT PCNT_ANIM_VS(Vertex_PCNT input)
{
	VertexOutput_PCNT output = (VertexOutput_PCNT)0;

	float4 pos = 0;
	float4 normal = 0;
	{
		pos = mul(input.Position, MeshAnimationMatrix[MeshIndex]);
		normal = mul(input.Normal, MeshAnimationMatrix[MeshIndex]);
	}

	float4 world = mul(pos, WorldTransformMatrix);
	float4 view = mul(world, ViewMatrix);
	float4 proj = mul(view, ProjectionMatrix);

	output.p = proj;
	output.n = normal;
	output.c = input.Color;
	output.t = input.Diffuse;

	return output;

}

VertexOutput_PCNT PCNT_SKIN_ANIM_VS(Vertex_PCNT_Skinning input)
{
	VertexOutput_PCNT output = (VertexOutput_PCNT)0;

	float4 pos = 0;
	float4 normal = 0;
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

	float4 world = mul(pos, WorldTransformMatrix);
	float4 view = mul(world, ViewMatrix);
	float4 proj = mul(view, ProjectionMatrix);

	output.p = proj;
	output.n = normal;
	output.c = input.Color;
	output.t = input.Diffuse;

	return output;
}

VertexOutput_PCNT_P PCNTs_VS(Vertex_PCNTs input)
{
	float4 pos = mul(input.Position, WorldTransformMatrix);
	pos = mul(pos, ViewMatrix);
	pos = mul(pos, ProjectionMatrix);

	VertexOutput_PCNT_P output = (VertexOutput_PCNT_P)0;
	output.p = pos;
	output.c = input.Color;
	output.n = input.Normal;
	output.t = input.Diffuse;
	//output.param.x = input.MaterialIndex;
	output.Index = input.MaterialIndex;

	return output;
}

VertexOutput_PCNT_P PCNTs_ANIM_VS(Vertex_PCNTs input)
{
	VertexOutput_PCNT_P output = (VertexOutput_PCNT_P)0;

	float4 pos = 0;
	float4 normal = 0;
	{
		pos = mul(input.Position, MeshAnimationMatrix[MeshIndex]);
		normal = mul(input.Normal, MeshAnimationMatrix[MeshIndex]);
	}

	float4 world = mul(pos, WorldTransformMatrix);
	float4 view = mul(world, ViewMatrix);
	float4 proj = mul(view, ProjectionMatrix);

	output.p = proj;
	output.n = normal;
	output.c = input.Color;
	output.t = input.Diffuse;
	output.Index = input.MaterialIndex;

	return output;

}

VertexOutput_PCNT_P PCNTs_SKIN_ANIM_VS(Vertex_PCNTs_Skinning input)
{
	VertexOutput_PCNT_P output = (VertexOutput_PCNT_P)0;

	float4 pos = 0;
	float4 normal = 0;
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

	float4 world = mul(pos, WorldTransformMatrix);
	float4 view = mul(world, ViewMatrix);
	float4 proj = mul(view, ProjectionMatrix);

	output.p = proj;
	output.n = normal;
	output.c = input.Color;
	output.t = input.Diffuse;
	output.Index = input.MaterialIndex;

	return output;
}