cbuffer SkyBoxCBuffer
{
	float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
};

struct VS_INPUT
{
	float4 ObjectPosition : POSITION;
	float2 TextureCoordinates :TEXTURECOORDINATE;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position: SV_POSITION;
	float3 TextureCoordinate : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT OUT = (VS_OUTPUT)0;

	OUT.Position = mul(IN.ObjectPosition, WorldViewProjection);
	OUT.TextureCoordinate = IN.ObjectPosition.xyz;

	return OUT;
}