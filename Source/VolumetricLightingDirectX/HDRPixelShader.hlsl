SamplerState ColorSampler;
Texture2D SceneTexture;

cbuffer HDRCBuffer
{
	float Exposure;
	float GammaCorrection;
	uint ApplyHDR;
	uint ApplyGammaCorrection;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float3 HDRColor = SceneTexture.Sample(ColorSampler, Input.TextureCoordinate).rgb ;

	float3 toneMappedColor = float3(1.0f, 1.0f, 1.0f) - exp(-HDRColor * Exposure);

	toneMappedColor = lerp(HDRColor, toneMappedColor, ApplyHDR);

	float3 GammaCorrectedColor;
	float InverseGamma = 1.0 / GammaCorrection;
	GammaCorrectedColor.x = pow(abs(toneMappedColor.x), InverseGamma);
	GammaCorrectedColor.y = pow(abs(toneMappedColor.y), InverseGamma);
	GammaCorrectedColor.z = pow(abs(toneMappedColor.z), InverseGamma);

	float3 OutputColor = lerp(toneMappedColor, GammaCorrectedColor, ApplyGammaCorrection);

	return float4(OutputColor, 1.0f);
}