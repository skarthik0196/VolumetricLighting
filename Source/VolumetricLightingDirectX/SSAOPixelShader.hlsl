Texture2D NormalTexture : register(t0);
Texture2D PositionTexture : register(t1);
Texture2D NoiseTexture : register(t2);
Texture2D DepthTexture : register(t3);

SamplerState ClampSampler : register(s2); //Point Sampling
SamplerState WrapSampler : register (s3);

static const uint KernelSize = 64;

cbuffer PSCBuffer
{
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float2 NoiseScale;
	float Radius;
	float Intensity;
	float4 SSAOKernel[64];
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

float3 GetViewSpaceNormal(float3 viewSpacePosition)
{
	return cross(normalize(ddx(viewSpacePosition)), normalize(ddy(viewSpacePosition)));
}

float AmbientOcclusion(float2 texCoord, float2 uv, float3 position, float3 normal)
{
	float3 diff = mul(float4(PositionTexture.Sample(ClampSampler, texCoord + uv).xyz, 1.0f), ViewMatrix).xyz - position;
	float3 diffN = normalize(diff);
	float d = length(diff) * 0.1f;		//scale
	return max(0.0f, dot(normal, diffN) - 0.025f) * (1.0f / 1.0 + d) * Intensity;  //bias , intensity
}

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float3 Position = mul(float4(PositionTexture.Sample(ClampSampler, Input.TextureCoordinate).xyz, 1.0f), ViewMatrix).xyz;
	float3 Normal = normalize(mul(float4(normalize(NormalTexture.Sample(ClampSampler, Input.TextureCoordinate).xyz), 0.0f), ViewMatrix).xyz);//GetViewSpaceNormal(Position);
	float3 RandomVector = NoiseTexture.Sample(WrapSampler, Input.TextureCoordinate * NoiseScale).xyz;

	//const float2 Vec[8] = { float2(1,0),float2(-1,0), float2(0,1),float2(0,-1), float2(1,1), float2(-1,-1), float2(1,-1), float2(-1,1) };
	//float Occlusion = 0.0f;
	//float RadiusAdjust = Radius / Position.z;

	//for (int j = 0; j < 8; ++j)
	//{
	//	float2 Coord1 = reflect(Vec[j], RandomVector) * RadiusAdjust;
	//	float2 Coord2 = float2(Coord1.x*0.707 - Coord1.y*0.707, Coord1.x*0.707 + Coord1.y*0.707);

	//	Occlusion += AmbientOcclusion(Input.TextureCoordinate, Coord1*0.25, Position, Normal);
	//	Occlusion += AmbientOcclusion(Input.TextureCoordinate, Coord2*0.5, Position, Normal);
	//	Occlusion += AmbientOcclusion(Input.TextureCoordinate, Coord1*0.75, Position, Normal);
	//	Occlusion += AmbientOcclusion(Input.TextureCoordinate, Coord2, Position, Normal);
	//}

	//Occlusion /= (8.0f*4.0f);
	//Occlusion = 1 - Occlusion;

	float3 Tangent = normalize(RandomVector - Normal * dot(RandomVector, Normal));
	float3 BiTangent = cross(Normal, Tangent);
	float3x3 TBN = float3x3(Tangent, BiTangent, Normal);

	float Occlusion = 0.0f;
	float RadiusAdjust = Radius / Position.z;

			//float3 sample_sphere[samples] = {
			//  float3( 0.5381, 0.1856,-0.4319), float3( 0.1379, 0.2486, 0.4430),
			//  float3( 0.3371, 0.5679,-0.0057), float3(-0.6999,-0.0451,-0.0019),
			//  float3( 0.0689,-0.1598,-0.8547), float3( 0.0560, 0.0069,-0.1843),
			//  float3(-0.0146, 0.1402, 0.0762), float3( 0.0100,-0.1924,-0.0344),
			//  float3(-0.3577,-0.5301,-0.4358), float3(-0.3169, 0.1063, 0.0158),
			//  float3( 0.0103,-0.5869, 0.0046), float3(-0.0897,-0.4940, 0.3287),
			//  float3( 0.7119,-0.0154,-0.0918), float3(-0.0533, 0.0596,-0.5411),
			//  float3( 0.0352,-0.0631, 0.5460), float3(-0.4776, 0.2847,-0.0271)
		 // };


	for (uint i = 0; i < KernelSize; ++i)
	{
		float3 SamplePosition = mul(SSAOKernel[i].xyz, TBN);
		SamplePosition = Position + SamplePosition * Radius;

		float4 Offset = float4(SamplePosition, 1.0f);
		Offset = mul(Offset, ProjectionMatrix);
		Offset.xy /= Offset.w;
		Offset.xy = Offset.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

		float SampleDepth = mul(PositionTexture.Sample(ClampSampler, Offset.xy), ViewMatrix).z;

		//float RangeCheck = abs(Position.z - SampleDepth) < Radius ? 1.0 : 0.0;
		 float RangeCheck = smoothstep(0.0f, 1.0f, Radius / abs(Position.z - SampleDepth));

		Occlusion += ((SampleDepth) <= (SamplePosition.z + 0.025f) ? 1.0f : 0.0f) * RangeCheck * Intensity;
		//Occlusion += (SampleDepth <= SamplePosition.z ? 1.0 : 0.0) * RangeCheck;
	}

	Occlusion = (1.0 - (Occlusion / KernelSize));

	return float4(Occlusion, Occlusion, Occlusion, 1.0f);
}