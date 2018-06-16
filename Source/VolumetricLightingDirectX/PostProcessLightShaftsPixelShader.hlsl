SamplerState Sampler;
Texture2D OcclusionMap;

#define SAMPLES 128

cbuffer GodRayCBuffer
{
	float2 LightPosition;
	float Exposure;
	float Decay;
	float Weight;
	float Padding1;
	float Padding2;
	float Padding3;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float4 Output = (float4)0;

	float2 TexCoord = Input.TextureCoordinate;
	float2 DeltaTexCoord = TexCoord - LightPosition;
	DeltaTexCoord = DeltaTexCoord / SAMPLES;

	float3 Color = OcclusionMap.Sample(Sampler, TexCoord);
	float IlluminationDecay = 1.0f;

	for (int i = 0; i < SAMPLES; ++i)
	{
		TexCoord -= DeltaTexCoord;
		float3 Sample = OcclusionMap.Sample(Sampler, TexCoord);
		Sample = Sample * IlluminationDecay * Weight;
		Color += Sample;
		IlluminationDecay *= Decay;

	}

	Output.rgb = Color * Exposure;
	Output.a = 1.0f;
	return Output;
}