#include "../ConstantHeader.txt"

struct PS_in
{
	float4 p : SV_POSITION;
	float4 w : TEXCOORD3;
	float4 c : COLOR0;
	float4 n : NORMAL0;
	float2 t : TEXCOORD0;
};

float4 psmain(PS_in input) : SV_Target
{
	return  g_txTexA.Sample(g_SampleWrap, input.t);
}

