Texture2D NormalTexture : register(t0);
Texture2D PositionTexture : register(t1);
Texture2D NoiseTexture : register(t2);
Texture2D DepthTexture : register(t3);

SamplerState ClampSampler : register(s2); //Point Sampling
SamplerState WrapSampler : register (s3);

#define NUM_DIRECTIONS 8
#define NUM_STEPS 6
#define PI 3.14159265f

static const uint KernelSize = 64;

static const float2 Resolution = float2(1366, 768);
static const float2 InverseResolution = 1 / Resolution;
static const float2 FocalLen = float2(1.0f / tan(0.785398 * 0.5f) * (1366 / 768), 1.0f / tan(0.785398 * 0.5f));
static const float MaxRadiusPixels = 50.0f / (Resolution.x / Resolution.y);
static const float TangentBias = tan(30.0 * PI / 180.0);

cbuffer PSCBuffer
{
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float2 NoiseScale;
	float Radius;
	float Intensity;
	float4 SSAOKernel[64];
};

float3 GetViewSpaceNormal(float3 viewSpacePosition)
{
	return cross(normalize(ddx(viewSpacePosition)), normalize(ddy(viewSpacePosition)));
}

float Length2(float3 v)
{
	return dot(v, v);
}

float3 MinDiff(float3 P, float3 Pr, float3 Pl)
{
	float3 V1 = Pr - P;
	float3 V2 = P - Pl;
	return (Length2(V1) < Length2(V2)) ? V1 : V2;
}

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

void ComputeSteps(inout float2 stepSize_UV, inout float numSteps, float rayRadius_Pixel, float rand)
{

	numSteps = min(NUM_STEPS, rayRadius_Pixel);

	float stepSize_Pixel = rayRadius_Pixel / (numSteps + 1);
	float maxNumSteps = MaxRadiusPixels / stepSize_Pixel;
	if (maxNumSteps < numSteps)
	{
		numSteps = floor(maxNumSteps + rand);
		numSteps = max(numSteps, 1);
		stepSize_Pixel = MaxRadiusPixels / numSteps;
	}

	stepSize_UV = stepSize_Pixel * InverseResolution;
}

float Falloff(float d2)
{
	return d2 * (-1.0f / (Radius *Radius)) + 1.0f;
}

float2 RotateDirections(float2 Dir, float2 CosSin)
{
	return float2(Dir.x*CosSin.x - Dir.y*CosSin.y,
		Dir.x*CosSin.y + Dir.y*CosSin.x);
}

float2 SnapUVOffset(float2 uv)
{
	return round(uv * Resolution) * InverseResolution;
}

float InvLength(float2 v)
{
	return rsqrt(dot(v, v));
}

float Tangent(float3 T)
{
	return -T.z * InvLength(T.xy);
}

float Tangent(float3 P, float3 S)
{
	return (P.z - S.z) * InvLength(S.xy - P.xy);
}

float BiasedTangent(float3 T)
{
	return Tangent(T) + TangentBias;
}

float3 TangentVector(float2 deltaUV, float3 dPdu, float3 dPdv)
{
	return deltaUV.x * dPdu + deltaUV.y * dPdv;
}

float TanToSin(float x)
{
	return x * rsqrt(x*x + 1.0f);
}

float IntegrateOcclusion(float2 uv0, float2 snapped_duv, float3 P, float3 dPdu, float3 dPdv, inout float tanH)
{
	float occlusion = 0;

	float3 T1 = TangentVector(snapped_duv, dPdu, dPdv);
	float tanT = BiasedTangent(T1);
	float sinT = TanToSin(tanT);

	float3 S = mul(float4(PositionTexture.SampleLevel(ClampSampler, snapped_duv, 0).rgb, 1.0f), ViewMatrix).rgb;
	float tanS = Tangent(P, S);

	float sinS = TanToSin(tanS);
	float d2 = Length2(S - P);

	float r2 = Radius * Radius;

	if ((d2 < r2) && (tanS > tanT))
	{
		occlusion = Falloff(d2) * (sinS - sinT);

		tanH = max(tanH, tanS);
	}

	return occlusion;
}

float HorizonOcclusion(float2 deltaUV, float2 texelDeltaUV, float2 uv0, float3 P, float numSteps, float randStep, float3 dPdu, float3 dPdv)
{
	float occlusion = 0.0f;

	float2 uv = uv0 + SnapUVOffset(randStep * deltaUV);

	deltaUV = SnapUVOffset(deltaUV);

	float3 T = deltaUV.x * dPdu + deltaUV.y * dPdv;

	float tanH = BiasedTangent(T);

	float2 snapped_dUV = SnapUVOffset(randStep * deltaUV + texelDeltaUV);
	occlusion = IntegrateOcclusion(uv0, snapped_dUV, P, dPdu, dPdv, tanH);
	--numSteps;

	float sinH = tanH / sqrt(1.0f + tanH * tanH);
	float r2 = Radius * Radius;

	for (float j = 1; j <= numSteps; ++j)
	{
		uv += deltaUV;
		float3 S = mul(float4(PositionTexture.SampleLevel(ClampSampler, uv, 0).rgb, 1.0f), ViewMatrix).rgb;
		float tanS = Tangent(P, S);
		float d2 = Length2(S - P);

		[branch]
		if ((d2 < r2) && (tanS > tanH))
		{
			float sinS = tanS / sqrt(1.0f + tanS * tanS);
			occlusion += Falloff(d2) * (sinS - sinH);

			tanH = tanS;
			sinH = sinS;
		}
	}

	return occlusion;
}


float4 main(VS_OUTPUT Input) : SV_TARGET
{

	float3 P = mul(float4(PositionTexture.Sample(ClampSampler, Input.TextureCoordinate).rgb, 1.0f), ViewMatrix).rgb;
	float3 Rand = NoiseTexture.Sample(WrapSampler, Input.TextureCoordinate * NoiseScale).rgb;

	float2 RayRadius_UV = 0.5f * Radius * FocalLen / P.z;
	float RayRadius_Pixel = RayRadius_UV.x * Resolution.x;
	if (RayRadius_Pixel < 1) return 1.0f;

	float NumSteps;
	float2 StepSize;

	ComputeSteps(StepSize, NumSteps, RayRadius_Pixel, Rand.z);

	float3 PRight, PLeft, PTop, PBottom;

	PRight = mul(float4(PositionTexture.Sample(ClampSampler, Input.TextureCoordinate + float2(InverseResolution.x, 0.0f)).rgb, 1.0f), ViewMatrix).rgb;
	PLeft = mul(float4(PositionTexture.Sample(ClampSampler, Input.TextureCoordinate + float2(-InverseResolution.x, 0.0f)).rgb, 1.0f), ViewMatrix).rgb;
	PTop = mul(float4(PositionTexture.Sample(ClampSampler, Input.TextureCoordinate + float2(0.0f, InverseResolution.y)).rgb, 1.0f), ViewMatrix).rgb;
	PBottom = mul(float4(PositionTexture.Sample(ClampSampler, Input.TextureCoordinate + float2(0.0f, -InverseResolution.y)).rgb, 1.0f), ViewMatrix).rgb;

	float3 DpDu = MinDiff(P, PRight, PLeft);
	float3 DpDv = MinDiff(P, PTop, PBottom) * (Resolution.y * InverseResolution.y);

	float Occlusion = 0.0f;
	float Alpha = 2.0f * PI / NUM_DIRECTIONS;

	float d;

	for (d = 0; d < NUM_DIRECTIONS; ++d)
	{
		float Angle = Alpha * d;
		float2 Dir = RotateDirections(float2(cos(Angle), sin(Angle)), Rand.xy);
		float2 DeltaUV = Dir * StepSize.xy;
		float2 TexelDeltaUV = Dir * InverseResolution;
		Occlusion += HorizonOcclusion(DeltaUV, TexelDeltaUV, Input.TextureCoordinate, P, NumSteps, Rand.z, DpDu, DpDv);
	}

	Occlusion = 1.0f - Occlusion / NUM_DIRECTIONS * Intensity;

	return float4(Occlusion, Occlusion, Occlusion, P.z);
}