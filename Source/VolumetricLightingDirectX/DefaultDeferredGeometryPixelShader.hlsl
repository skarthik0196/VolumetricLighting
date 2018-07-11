SamplerState Sampler;
Texture2D ColorTexture;
Texture2D NormalTexture;
Texture2D MetallicTexture;
Texture2D RoughnessTexture;

struct PS_OUTPUT
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 WorldPosition : SV_TARGET2;
	float4 OcclusionMap : SV_TARGET3;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : WORLDPOS;
	float2 TextureCoordinate : TEXTURECOORDINATE;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
};

cbuffer CBufferPerObject
{
	float4x4 WorldMatrix : WORLDMATRIX;
	float4x4 WorldViewProjectionMatrix : WVP;
};

PS_OUTPUT main(VS_OUTPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float4 Albedo = ColorTexture.Sample(Sampler, Input.TextureCoordinate);
	float4 NormalMap = NormalTexture.Sample(Sampler, Input.TextureCoordinate);
	float4 Metallic = MetallicTexture.Sample(Sampler, Input.TextureCoordinate);;
	float4 Roughness = RoughnessTexture.Sample(Sampler, Input.TextureCoordinate);

	NormalMap = (NormalMap * 2.0f) - 1.0f;

	float3x3 TBN = float3x3(normalize(Input.Tangent), normalize(Input.BiNormal), normalize(Input.Normal));
	NormalMap = float4(/*normalize*/(mul(NormalMap.xyz, TBN)), 0.0f);

	Output.Color = float4(Albedo.rgb, Metallic.r);
	Output.Normal = float4(NormalMap.xyz /*Input.Normal*/, Roughness.r);
	Output.WorldPosition = Input.WorldPosition;
	Output.OcclusionMap = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return Output;
}