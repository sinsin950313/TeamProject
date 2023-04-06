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

[maxvertexcount(3)]
void GS(triangle GSInput input[3], inout TriangleStream<GSInput> triStream)
{
	GSInput output;
	for(int i = 0; i < 3; ++i)
	{
		output.Projection = input[i].Projection;
		output.Position = input[i].Position;

		triStream.Append(output);
	}
	triStream.RestartStrip();

}
