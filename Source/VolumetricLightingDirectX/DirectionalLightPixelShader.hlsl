SamplerState Sampler;

Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D PositionTexture : register(t2);

cbuffer DirectionalLightData
{
	float4 AmbientColor;
	float4 DLightColor;
	float3 DLightDirection;
	float Padding;
	float3 CameraPosition;
};

struct VS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

float4 main( VS_INPUT Input ) : SV_TARGET
{
	float4 OutputColor = (float4)0;

	float3 WorldPosition = (PositionTexture.Sample(Sampler, Input.TextureCoordinate)).xyz;
	float3 Normal = (NormalTexture.Sample(Sampler, Input.TextureCoordinate)).xyz;

	float3 ViewDirection = normalize(CameraPosition - WorldPosition);
	float3 NNormal = normalize(Normal);
	float3 DirectionToDLight = -DLightDirection;

	float NormalDotDirection = dot(NNormal, DirectionToDLight);
	float3 HalfVector = normalize(DirectionToDLight + ViewDirection);
	float NormalDotHalf = dot(NNormal, HalfVector);

	float4 LightingCoefficients = lit(NormalDotDirection, NormalDotHalf, 128);


	float4 Color = ColorTexture.Sample(Sampler, Input.TextureCoordinate);
	float3 Ambient = Color.rgb * AmbientColor.rgb * LightingCoefficients.x;
	float3 Diffuse = DLightColor.rgb * Color.rgb * LightingCoefficients.y;

	OutputColor.rgb = Ambient + Diffuse;
	OutputColor.a = 1.0f;

	return OutputColor;
}