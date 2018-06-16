SamplerState Sampler;
Texture2D Texture;

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	return Texture.Sample(Sampler, Input.TextureCoordinate);
}