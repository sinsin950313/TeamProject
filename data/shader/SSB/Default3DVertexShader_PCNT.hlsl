#include "VertexShaderInputType.hlsli"
#include "SSBConstantBuffer.hlsli"
#include "VertexShaderOutputType.hlsli"

VertexOutput_PCNT VS(Vertex_PCNT input)
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

