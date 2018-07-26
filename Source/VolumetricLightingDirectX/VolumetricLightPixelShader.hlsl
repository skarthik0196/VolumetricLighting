Texture2D PositionTexture : register(t0);
Texture2D ShadowMapTexture : register (t1);
SamplerState PointClampSampler : register(s3);

#define PI 3.14159265f
#define NUM_SAMPLES 128

cbuffer PSData
{
	float4x4 LightWorldViewProjection;
	float3 CameraWorldPosition;
	float GScattering;
	float4 LightColor;
	float3 DLightDirection;
	float ScatteringAmount;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

static const float DitherMatrix[16] = { 0.0f,0.5f,0.125f,0.625f, 0.75f,0.22f,0.875f,0.375f, 0.1875f, 0.6875f, 0.0625f, 0.5625f, 0.9375f, 0.4375f, 0.8125f, 0.3125f };

float ComputeScattering(float lightDotView)
{
	float scattering = 1.0f - (GScattering * GScattering);
	scattering /= (4.0 * PI * pow(abs(1.0f + (GScattering * GScattering) - (2.0f * GScattering * lightDotView)), 1.5f));
	return scattering * ScatteringAmount;
}

float GetDitherValue(uint2 pixelPosition)
{
	int x = int(pixelPosition.x % 4);
	int y = int(pixelPosition.y % 4);
	return DitherMatrix[x + (y * 4)];
}

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float3 LightDirection = -DLightDirection;
	float3 WorldPosition = PositionTexture.Sample(PointClampSampler, Input.TextureCoordinate).xyz;
	float3 RayStartPosition = CameraWorldPosition;

	float3 RayVector = WorldPosition - RayStartPosition;
	float RayLength = length(RayVector);
	float3 RayDirection = normalize(RayVector);

	float StepLength = RayLength / NUM_SAMPLES;
	float3 Step = RayDirection * StepLength;

	float3 CurrentPosition = RayStartPosition;
	float3 AccumulateScattering = 0.0f.xxx;

	float Dither = GetDitherValue(uint2(Input.Position.xy));

	for (uint i = 0; i < NUM_SAMPLES; ++i)
	{
		float4 LightProjection = mul(float4(CurrentPosition, 1.0f), LightWorldViewProjection);
		LightProjection.y = -LightProjection.y;
		LightProjection.xyz /= LightProjection.w;

		float2 shadowCoordinates = LightProjection.xy * 0.5f + 0.5f;

		float ShadowValue = ShadowMapTexture.Sample(PointClampSampler, shadowCoordinates).r;

		if (ShadowValue > LightProjection.z)
		{
			AccumulateScattering += ComputeScattering(dot(LightDirection, RayDirection)) * LightColor.rgb;
		}

		CurrentPosition += Step;// *Dither;
	}

	AccumulateScattering /= NUM_SAMPLES;

	return float4(AccumulateScattering, 1.0f);
}