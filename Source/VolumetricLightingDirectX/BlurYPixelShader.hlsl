SamplerState Sampler : register(s0);
Texture2D InputTexture;

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

static const float Weights[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };
static const float KernelOffsets[3] = { 0.0f, 1.3846153846f, 3.2307692308f };

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float3 Color = 0.0f.xxx;

	float2 Dimensions;
	InputTexture.GetDimensions(Dimensions.x, Dimensions.y);

	for (uint i = 0; i < 3; ++i)
	{
		Color += InputTexture.Sample(Sampler, Input.TextureCoordinate + float2(0.0f, KernelOffsets[i] / Dimensions.y)).xyz * Weights[i];
		Color += InputTexture.Sample(Sampler, Input.TextureCoordinate - float2(0.0f, KernelOffsets[i] / Dimensions.y)).xyz * Weights[i];
	}

	return float4(Color, 1.0f);
}