cbuffer CBufferPerObject
{
	float4x4 LightWVP : WVP;
};

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 TextureCoordinate :TEXTURECOORDINATE;
	float3 Normal : NORMAL;
};

float4 main(VS_INPUT Input) : SV_POSITION
{

	float4 OutputPosition = mul(Input.Position, LightWVP);
	return OutputPosition;
}