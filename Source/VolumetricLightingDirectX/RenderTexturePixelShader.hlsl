SamplerState Sampler;
Texture2D Texture;

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float4 OutputColor = float4(Texture.Sample(Sampler, Input.TextureCoordinate).rgb,1.0f);
	return OutputColor;
}