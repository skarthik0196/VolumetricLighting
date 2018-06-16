SamplerState Sampler;
Texture2D ColorTexture;

struct PS_OUTPUT
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 WorldPosition : SV_TARGET2;
	float4 OcclusionMap : SV_TARGET3;
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
	PS_OUTPUT Output = (PS_OUTPUT)0;

	Output.Color = ColorTexture.Sample(Sampler, Input.TextureCoordinate);
	Output.Normal = normalize(float4(Input.Normal, 0.0f));
	Output.WorldPosition = Input.WorldPosition;
	Output.OcclusionMap = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return Output;
}