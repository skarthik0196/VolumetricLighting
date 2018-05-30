Texture2D DiffuseTexture;
SamplerState DiffuseSampler;

cbuffer DirectionalLightData
{
	float4 AmbientColor;
	float4 DLightColor;
	float3 DLightDirection;
	float Padding;
	float3 CameraPosition;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 WorldPosition : WORLDPOS;
	float2 TextureCoordinate : TEXTURECOORDINATE;
	float3 Normal : NORMAL;
};

float4 main(VS_OUTPUT IN) : SV_TARGET
{
	float4 OutputColor = (float4)0;

	float3 ViewDirection = normalize(CameraPosition - IN.WorldPosition);
	float3 NNormal = normalize(IN.Normal);
	float3 DirectionToDLight = -DLightDirection;

	float NormalDotDirection = dot(NNormal, DirectionToDLight);
	float3 HalfVector = normalize(DirectionToDLight + ViewDirection);
	float NormalDotHalf = dot(NNormal, HalfVector);

	float4 LightingCoefficients = lit(NormalDotDirection, NormalDotHalf, 128);


	float4 Color = DiffuseTexture.Sample(DiffuseSampler, IN.TextureCoordinate);
	float3 Ambient = Color.rgb * AmbientColor.rgb * LightingCoefficients.x;
	float3 Diffuse = DLightColor.rgb * Color.rgb * LightingCoefficients.y;

	OutputColor.rgb = Ambient + Diffuse;
	OutputColor.a = 1.0f;

	return OutputColor;
}