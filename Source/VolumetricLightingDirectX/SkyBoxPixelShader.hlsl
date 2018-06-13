TextureCube SkyBoxTexture;
SamplerState TriLinearSampler;

struct VS_OUTPUT
{
	float4 Position: SV_POSITION;
	float3 TextureCoordinate : TEXCOORD;
};

float4 main(VS_OUTPUT IN) : SV_TARGET
{
	return SkyBoxTexture.Sample(TriLinearSampler, IN.TextureCoordinate);
}