Texture2D TextureColor : register(t0);
sampler TextureSamplerColor : register(s0);

struct PS_INPUT
{
	float4 p : SV_POSITION;
	float4 w : TEXCOORD3;
	float4 n : NORMAL0;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

struct PixelOutput
{
	float4 Position : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Color : SV_TARGET2;
};

PixelOutput PS(PS_INPUT input)
{
	PixelOutput output = (PixelOutput)0;
	output.Position = input.w;
	output.Normal = float4(input.n.xyz, 1);
	output.Color = TextureColor.Sample(TextureSamplerColor, input.t);
	return output;
}
