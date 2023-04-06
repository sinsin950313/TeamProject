struct GSInput
{
	float4 Projection : SV_POSITION;
	float4 Position : TEXCOORD1;
};

cbuffer LightData : register(b9)
{
	matrix LightWorldMatrix;
	matrix LightViewMatrix;
	matrix LightProjMatrix;
};

struct PSInput
{
	float4 Projection : SV_POSITION;
};

[maxvertexcount(4)]
void GS(triangle GSInput input[3], inout TriangleStream<PSInput> triStream)
{
	PSInput output;
	for(int i = 0; i < 3; ++i)
	{
		output.Projection = input[i].Projection;

		triStream.Append(output);
	}
	triStream.RestartStrip();
}

float4 PS(PSInput input) : SV_TARGET
{
	return input.Projection;
}
