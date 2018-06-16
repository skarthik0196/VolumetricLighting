SamplerState Sampler;

struct PS_OUTPUT
{
	float4 OcclusionMap : SV_TARGET0;
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
	Output.OcclusionMap = float4(1.0f, 1.0f, 1.0f, 1.0f);

	return Output;
}