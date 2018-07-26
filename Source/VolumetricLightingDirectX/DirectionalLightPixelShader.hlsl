SamplerState Sampler;
SamplerComparisonState ShadowSampler;

Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D PositionTexture : register(t2);
Texture2D ShadowMap : register(t3);
Texture2D SSAOTexture : register(t4);

cbuffer DirectionalLightData
{
	float4x4 LightViewProjectionMatrix;
	float4 AmbientColor;
	float4 DLightColor;
	float3 DLightDirection;
	float Padding;
	float3 CameraPosition;
};

static const float PI = 3.14159265359;

struct VS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
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
	k = (k * k) / 8.0f;

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

float BlinnPhongSpecular(float3 normal, float3 halfWayVector, uint specularPower)
{
	return pow(max(dot(normal, halfWayVector), 0.0), specularPower);
}

float CalculateShadowFactor(float2 projectionTextureCoordinates, float shadowMapWidth, float shadowMapHeight, float depthValue)
{
	float2 texelOffsets = float2(1.0f / shadowMapWidth, 1.0f / shadowMapHeight);

	float shadowFactor = 0.0f;

	shadowFactor += ShadowMap.SampleCmpLevelZero(ShadowSampler, projectionTextureCoordinates.xy, depthValue, int2(-1, -1));
	shadowFactor += ShadowMap.SampleCmpLevelZero(ShadowSampler, projectionTextureCoordinates.xy, depthValue, int2(-1, 0));
	shadowFactor += ShadowMap.SampleCmpLevelZero(ShadowSampler, projectionTextureCoordinates.xy, depthValue, int2(-1, 1));

	shadowFactor += ShadowMap.SampleCmpLevelZero(ShadowSampler, projectionTextureCoordinates.xy, depthValue, int2(0, -1));
	shadowFactor += ShadowMap.SampleCmpLevelZero(ShadowSampler, projectionTextureCoordinates.xy, depthValue, int2(0, 0));
	shadowFactor += ShadowMap.SampleCmpLevelZero(ShadowSampler, projectionTextureCoordinates.xy, depthValue, int2(0, 1));

	shadowFactor += ShadowMap.SampleCmpLevelZero(ShadowSampler, projectionTextureCoordinates.xy, depthValue, int2(1, -1));
	shadowFactor += ShadowMap.SampleCmpLevelZero(ShadowSampler, projectionTextureCoordinates.xy, depthValue, int2(1, 0));
	shadowFactor += ShadowMap.SampleCmpLevelZero(ShadowSampler, projectionTextureCoordinates.xy, depthValue, int2(1, 1));

	shadowFactor = saturate(shadowFactor / 9);
	return shadowFactor;
}

float4 main( VS_INPUT Input ) : SV_TARGET
{
	float4 OutputColor = (float4)0;

	// Phong Begin

	/*float3 WorldPosition = (PositionTexture.Sample(Sampler, Input.TextureCoordinate)).xyz;
	float3 Normal = (NormalTexture.Sample(Sampler, Input.TextureCoordinate)).xyz;
	float4 LightProjectionPosition = mul(float4(WorldPosition, 1.0f), LightViewProjectionMatrix);

	float DepthBias = 0.005f;
	bool InLight = false;

	LightProjectionPosition.y = -LightProjectionPosition.y;
	float2 ProjectionCoordinates = (LightProjectionPosition.xy / LightProjectionPosition.w);
	ProjectionCoordinates = (ProjectionCoordinates * 0.5f) + 0.5f;
	float DepthValue = LightProjectionPosition.z / LightProjectionPosition.w;

	if (saturate(ProjectionCoordinates.x == ProjectionCoordinates.x) && saturate(ProjectionCoordinates.y == ProjectionCoordinates.y))
	{
		float SampledDepth = ShadowMap.Sample(Sampler, ProjectionCoordinates.xy).r;
		SampledDepth += DepthBias;
		InLight = (DepthValue < SampledDepth);
	}

	float3 ViewDirection = normalize(CameraPosition - WorldPosition);
	float3 NNormal = normalize(Normal);
	float3 DirectionToDLight = -DLightDirection;

	float NormalDotDirection = dot(NNormal, DirectionToDLight);
	float3 HalfVector = normalize(DirectionToDLight + ViewDirection);
	float NormalDotHalf = dot(NNormal, HalfVector);

	float4 LightingCoefficients = lit(NormalDotDirection, NormalDotHalf, 128);


	float3 Color = ColorTexture.Sample(Sampler, Input.TextureCoordinate).rgb;
	float3 Ambient = Color.rgb * AmbientColor.rgb * LightingCoefficients.x;
	float3 Diffuse = DLightColor.rgb * Color.rgb * LightingCoefficients.y;

	Diffuse = lerp(0.0f, Diffuse, InLight);

	OutputColor.rgb = Ambient + Diffuse;
	OutputColor.a = 1.0f;*/

	//Phong End

	// PBR Begin

	float3 L0 = (float3)0;

	float4 GBuffer1 = ColorTexture.Sample(Sampler, Input.TextureCoordinate);
	float3 Albedo = GBuffer1.xyz;
	Albedo = pow(Albedo, 2.2);
	float3 Metallic = GBuffer1.w;

	float4 GBuffer2 = (NormalTexture.Sample(Sampler, Input.TextureCoordinate));
	float3 Normal = GBuffer2.xyz;
	float Roughness = GBuffer2.w;

	float3 WorldPosition = (PositionTexture.Sample(Sampler, Input.TextureCoordinate)).xyz;

	float3 NNormal = normalize(Normal);
	float3 ViewDirection = normalize(CameraPosition - WorldPosition);
	float3 DirectionToDLight = -DLightDirection;
	float3 HalfVector = normalize(DirectionToDLight + ViewDirection);

	//Shadowing 

	float ShadowMapWidth, ShadowMapHeight;
	ShadowMap.GetDimensions(ShadowMapWidth, ShadowMapHeight);

	float4 LightProjectionPosition = mul(float4(WorldPosition, 1.0f), LightViewProjectionMatrix);
	float DepthBias = max(0.005f * (1.0f - dot(NNormal, DirectionToDLight)), 0.005f);

	bool InLight = false;
	float ShadowFactor = 0.0f;

	LightProjectionPosition.y = -LightProjectionPosition.y;
	float2 ProjectionCoordinates = (LightProjectionPosition.xy / LightProjectionPosition.w);
	ProjectionCoordinates = (ProjectionCoordinates * 0.5f) + 0.5f;
	float DepthValue = LightProjectionPosition.z / LightProjectionPosition.w;

	if (saturate(ProjectionCoordinates.x == ProjectionCoordinates.x) && saturate(ProjectionCoordinates.y == ProjectionCoordinates.y))
	{
		//float SampledDepth = ShadowMap.Sample(Sampler, ProjectionCoordinates.xy).r;
		DepthValue -= DepthBias;
		ShadowFactor = CalculateShadowFactor(ProjectionCoordinates, ShadowMapWidth, ShadowMapHeight, DepthValue);
		//InLight = (DepthValue < SampledDepth);
	}

	//End Shadowing

	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, Albedo, Metallic);

	// Calculate DFG/4(w0.n)(wi.n)

	float3 F = FresnelSchlick(max(dot(HalfVector, ViewDirection), 0.0f), F0);
	float D = DistributionGGX(NNormal, HalfVector, Roughness);
	float G = GeometrySmith(NNormal, ViewDirection, DirectionToDLight, Roughness);

	float3 Numerator = D * F * G;
	float Denominator = 4 * max(dot(ViewDirection, NNormal), 0.0f) * max(dot(NNormal, DirectionToDLight), 0.0f);
	float3 Specular = Numerator / max(Denominator, 0.001);

	float3 Kd = (float3)1.0f - F;
	Kd *= (1.0 - Metallic);

	float NormalDotDirection = max(dot(NNormal, DirectionToDLight), 0.0);
	float3 Radiance = DLightColor.rgb;

	//
	//float Specular_Blinn = BlinnPhongSpecular(NNormal, HalfVector, 16);
	//

	L0 = (((Kd * Albedo) / PI) + Specular) * Radiance * NormalDotDirection;

	float3 Diffuse = lerp(0.0f, L0, ShadowFactor);

	float AOFactor = SSAOTexture.Sample(Sampler, Input.TextureCoordinate).r;
	float3 Ambient = Albedo.rgb * AmbientColor.rgb * AOFactor;
	
	OutputColor.rgb = Diffuse + Ambient;

	OutputColor.rgb = OutputColor.rgb / (OutputColor.rgb + float3(1.0f, 1.0f, 1.0f));
	OutputColor.rgb = pow(OutputColor.rgb, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

	//OutputColor.rgb += Ambient;

	//Test
	//OutputColor.rgb = SSAOTexture.Sample(Sampler, Input.TextureCoordinate).rgb;//float3(AOFactor, AOFactor, AOFactor);

	OutputColor.a = 1.0f;
	// PBR end

	return OutputColor;
}