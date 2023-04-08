struct GSInput
{
	float4 Projection : SV_POSITION;
	float3 Position : TEXCOORD3;
};

cbuffer LightData : register(b9)
{
	matrix LightWorldMatrix;
	matrix LightViewMatrix;
	matrix LightProjMatrix;
};

struct GSOut
{
	float4 Projection : SV_POSITION;
};

[maxvertexcount(3)]
void GS(triangle GSInput input[3], inout TriangleStream<GSOut> triStream)
{
	GSOut output;

	for(int i = 0; i < 3; ++i)
	{
		float4 tmp = mul(float4(input[i].Position, 1), LightViewMatrix);
		output.Projection = mul(tmp, LightProjMatrix);

		triStream.Append(output);
	}
	triStream.RestartStrip();
}

float4 PS(float4 Projection : SV_POSITION) : SV_TARGET
{
	return Projection;
}
