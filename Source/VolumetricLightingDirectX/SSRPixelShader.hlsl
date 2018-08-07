SamplerState PointClampSampler : register(s3);

Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D PositionTexture : register(t2);
Texture2D DepthTexture : register(t3);

static const float MinRayStep = 0.01f;
static const int MaxSteps = 16;
static const float RayStep = 1.18f;
static const int BinarySearchSteps = 16;
static const float RayHitThreshold = 0.9f;

cbuffer PSCBuffer
{
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4 CameraPosition;
};

float4 SSRRayMarch(float3 direction, float3 hitCoord)
{
	float depth;

	for (int i = 0; i < MaxSteps; ++i)
	{
		hitCoord += direction;
		float4 projectedCoord = mul(float4(hitCoord, 1.0f), ProjectionMatrix);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	}
}

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};



float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float4 WorldPosition = PositionTexture.Sample(PointClampSampler, Input.TextureCoordinate);
	float4 WorldNormal = NormalTexture.Sample(PointClampSampler, Input.TextureCoordinate);

	float4 ViewPosition = mul(WorldPosition, ViewMatrix);
	float4 ViewNormal = mul(WorldPosition, ViewMatrix);

	float3 ReflectedVector = reflect(ViewPosition.xyz, ViewNormal.xyz);
	float3 ReflectedDirection = normalize(ReflectedVector);

	float3 HitPosition = ViewPosition.xyz;

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}