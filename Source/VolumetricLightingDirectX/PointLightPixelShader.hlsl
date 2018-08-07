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

static const float PI = 3.14159265359;

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

float3 FresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0);
}

float DistributionGGX(float3 normalVector, float3 halfWayVector, float roughness)
{
	float roughnessSquared = roughness * roughness;
	float a2 = roughnessSquared * roughnessSquared;

	float normalDotHalf = max(dot(normalVector, halfWayVector), 0.0f);
	float normalDotHalfSquared = normalDotHalf * normalDotHalf;
	float denominator = (normalDotHalfSquared * (a2 - 1.0)) + 1.0;
	denominator = denominator * denominator * PI;

	float numerator = a2;

	return numerator / denominator;
}

float GeometrySchlickGGX(float nDotV, float roughness)
{
	float k = roughness + 1;
	k = (k * k) / 8;

	float numerator = nDotV;
	float denominator = (nDotV*(1 - k)) + k;

	return numerator / denominator;
}

float GeometrySmith(float3 normal, float3 viewDirection, float3 directionToLight, float roughness)
{
	float nDotV = max(dot(normal, viewDirection), 0.0);
	float nDotL = max(dot(normal, directionToLight), 0.0);

	float ggx1 = GeometrySchlickGGX(nDotV, roughness);
	float ggx2 = GeometrySchlickGGX(nDotL, roughness);

	return ggx1 * ggx2;
}

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float4 OutputColor = (float4)0;

	float2 TextureCoord = float2(Input.Position.x, Input.Position.y) / ScreenResolution;

	//Phong begin

	//float3 WorldPosition = (PositionTexture.Sample(Sampler, TextureCoord)).xyz;
	//float3 Normal = normalize((NormalTexture.Sample(Sampler, TextureCoord)).xyz);
	//float3 Color = ColorTexture.Sample(Sampler, TextureCoord).rgb;

	//float3 LightDirection = (PointLightPosition.xyz - WorldPosition);
	//float Attenuation = saturate(1.0f - (length(LightDirection) / Radius));
	//float NormalDotDirection = dot(Normal, normalize(LightDirection));
	//float4 LightCoefficients = lit(NormalDotDirection, 0, 128);

	//OutputColor.rgb = Color.rgb * PointLightColor.rgb * LightCoefficients.y * Attenuation;
	//OutputColor.a = 1.0f;

	//Phong end

	//PBR Begin

	float3 L0 = (float3)0;

	float4 GBuffer1 = ColorTexture.Sample(Sampler, TextureCoord);
	float3 Albedo = GBuffer1.xyz;
	Albedo = pow(Albedo, 2.2);
	float3 Metallic = GBuffer1.w;

	float4 GBuffer2 = (NormalTexture.Sample(Sampler, TextureCoord));
	float3 Normal = GBuffer2.xyz;
	float Roughness = GBuffer2.w;

	float3 WorldPosition = (PositionTexture.Sample(Sampler, TextureCoord)).xyz;

	float3 F0 = 0.04.xxx;
	F0 = lerp(F0, Albedo, Metallic);

	float3 NNormal = normalize(Normal);
	float3 ViewDirection = normalize(CameraPosition - WorldPosition);

	float3 LightDirection = normalize(PointLightPosition.xyz - WorldPosition);
	float3 HalfVector = normalize(ViewDirection + LightDirection);
	float Distance = length(PointLightPosition.xyz - WorldPosition);

	//float Attenuation = saturate(1.0f - ((Distance*Distance) / (Radius*Radius)));  // 1.0f / (Distance * Distance); 
	float AttenuationNumerator = pow(saturate(1 - pow((Distance / Radius), 4)), 2);
	float AttenuationDenominator = (Distance * Distance) + 1;
	float Attenuation = AttenuationNumerator / AttenuationDenominator;

	float3 Radiance = PointLightColor.rgb * Attenuation;

	float3 F = FresnelSchlick(max(dot(HalfVector, ViewDirection), 0.0f), F0);
	float D = DistributionGGX(NNormal, HalfVector, Roughness);
	float G = GeometrySmith(NNormal, ViewDirection, LightDirection, Roughness);

	float3 Numerator = D * F * G;
	float Denominator = 4 * max(dot(ViewDirection, NNormal), 0.0f) * max(dot(NNormal, LightDirection), 0.0f);
	float3 Specular = Numerator / max(Denominator, 0.001);

	float3 Kd = (float3)1.0f - F;
	Kd *= (1.0 - Metallic);

	float NormalDotDirection = max(dot(NNormal, LightDirection), 0.0);

	L0 = (((Kd * Albedo) / PI) + Specular) * Radiance * NormalDotDirection;
	
	OutputColor.rgb = L0;

	OutputColor.rgb = OutputColor.rgb / (OutputColor.rgb + float3(1.0f, 1.0f, 1.0f));
	OutputColor.rgb = pow(OutputColor.rgb, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

	OutputColor.a = 1.0f;

	//PBR end

	return OutputColor;
}