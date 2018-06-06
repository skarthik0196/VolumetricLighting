SamplerState Sampler;
Texture2D ColorTexture;
Texture2D NormalTexture;
Texture2D PositionTexture;

cbuffer PointLightData
{
	float4 PointLightColor;
	float4 PointLightPosition;
	float3 CameraPosition;
	float Radius;
	float2 ScreenResolution;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};


float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float4 OutputColor = (float4)0;

	float2 TextureCoord = float2(Input.Position.x, Input.Position.y) / ScreenResolution;

	float3 WorldPosition = (PositionTexture.Sample(Sampler, TextureCoord)).xyz;
	float3 Normal = normalize((NormalTexture.Sample(Sampler, TextureCoord)).xyz);
	float4 Color = ColorTexture.Sample(Sampler, TextureCoord);

	float3 LightDirection = (PointLightPosition.xyz - WorldPosition);
	float Attenuation = saturate(1.0f - (length(LightDirection) / Radius));

	float NormalDotDirection = dot(Normal, normalize(LightDirection));

	float4 LightCoefficients = lit(NormalDotDirection, 0, 128);


	OutputColor.rgb = Color.rgb * PointLightColor.rgb * LightCoefficients.y * Attenuation;
	OutputColor.a = 1.0f;


	return OutputColor;
}