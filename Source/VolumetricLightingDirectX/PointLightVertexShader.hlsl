cbuffer CBufferPerObject
{
	float4x4 WorldViewProjectionMatrix : WVP;
};

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 TextureCoordinate :TEXTURECOORDINATE;
	float3 Normal :Normal;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	Output.Position = mul(IN.Position, WorldViewProjectionMatrix);

	return Output;
}