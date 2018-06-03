SamplerState Sampler;
Texture2D ColorTexture;

struct PS_OUTPUT
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 WorldPosition : SV_TARGET2;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : WORLDPOS;
	float2 TextureCoordinate : TEXTURECOORDINATE;
	float3 Normal : NORMAL;
};

PS_OUTPUT main(VS_OUTPUT Input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.Color = ColorTexture.Sample(Sampler, Input.TextureCoordinate);
	output.Normal = float4(Input.Normal.xyz, 1.0f);
	output.WorldPosition = Input.WorldPosition;

	return output;
}