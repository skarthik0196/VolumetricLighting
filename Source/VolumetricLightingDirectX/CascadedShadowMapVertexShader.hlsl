cbuffer CBufferPerObject
{
	float4x4 LightWVP[3] : WVP;
};

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 TextureCoordinate :TEXTURECOORDINATE;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 LightPosition : SV_POSITION;
	float4 LightPosition1 : POSITION1; 
	float4 LightPosition2 : POSITION2;
	float4 LightPosition3 : POSITION3;
};

VS_OUTPUT main(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	Output.LightPosition1 = mul(Input.Position, LightWVP[0]);
	Output.LightPosition2 = mul(Input.Position, LightWVP[1]);
	Output.LightPosition3 = mul(Input.Position, LightWVP[2]);

	Output.LightPosition = Output.LightPosition3;

	return Output;
}