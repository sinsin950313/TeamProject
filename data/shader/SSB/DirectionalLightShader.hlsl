struct VSInput
{

};

struct VSOutput
{

};

VSOutput VS(VSInput input)
{

}

cbuffer LightData : register(b9)
{
	matrix LightWorldMatrix;
	matrix LightViewMatrix;
	matrix LightProjMatrix;
};
