Texture2D DiffuseTexture;
SamplerState DiffuseSampler;

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : WORLDPOS;
	float2 TextureCoordinate : TEXTURECOORDINATE;
	float3 Normal : NORMAL;
};

float4 main(VS_OUTPUT IN) : SV_TARGET
{
	float4 OutputColor = (float4)0;

	OutputColor = DiffuseTexture.Sample(DiffuseSampler, IN.TextureCoordinate);

	return OutputColor;
}