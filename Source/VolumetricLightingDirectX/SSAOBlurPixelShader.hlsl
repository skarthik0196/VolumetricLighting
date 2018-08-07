SamplerState Sampler : register(s2); //Point Sampler
Texture2D SSAOTexture;

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float2 Dimensions;
	SSAOTexture.GetDimensions(Dimensions.x, Dimensions.y);

	float2 TexelSize = 1.0f / Dimensions;

	float Result = 0.0f;

	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(-2, -2)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(-2, -1)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(-2, 0)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(-2, 1)).r;
	
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(-1, -2)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(-1, -1)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(-1, 0)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(-1, 1)).r;
	
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(0, -2)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(0, -1)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(0, 0)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(0, 1)).r;
	
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(1, -2)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(1, -1)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(1, 0)).r;
	Result += SSAOTexture.Sample(Sampler, Input.TextureCoordinate, int2(1, 1)).r;

	Result = Result / 16.0f;


	//Test
	//Result = float4(SSAOTexture.Sample(Sampler, Input.TextureCoordinate).rgb, 1.0f);

	//return Result;
	//Test end

	return float4(Result, Result, Result, Result);
}