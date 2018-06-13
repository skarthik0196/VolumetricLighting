cbuffer CBufferPerObject
{
	float4x4 WorldMatrix : WORLDMATRIX;
	float4x4 WorldViewProjectionMatrix : WVP;
};

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 TextureCoordinate :TEXTURECOORDINATE;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : WORLDPOS;
	float2 TextureCoordinate : TEXTURECOORDINATE;
	float3 Normal : NORMAL;
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	Output.Position = mul(IN.Position, WorldViewProjectionMatrix);
	Output.WorldPosition = mul(IN.Position, WorldMatrix);
	Output.TextureCoordinate = IN.TextureCoordinate;
	Output.Normal = normalize(mul(float4(IN.Normal, 0.0f), WorldMatrix).xyz);

	return Output;
}