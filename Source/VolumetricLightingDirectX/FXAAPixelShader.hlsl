#define EDGE_THRESHOLD_MIN 0.0312
#define EDGE_THRESHOLD_MAX 0.125
#define ITERATIONS 8
#define SUBPIXEL_QUALITY 0.75

float QualityValues[12] = { 1.5f, 2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f };
#define QUALITY(i) QualityValues[i]

Texture2D SceneTexture;
SamplerState ColorSampler;

cbuffer FXAAData : register(b0)
{
	float2 ScreenResolution;
	float2 Padding;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

float RGBToLuma(float3 rgb)
{
	return dot(rgb, float3(0.299f, 0.587f, 0.114f));
}

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float4 OutputColor = (float4)0;
	//OutputColor = SceneTexture.Sample(ColorSampler, Input.TextureCoordinate);
	//return OutputColor;

	/*float2 InverseScreenSize = float2(1/ScreenResolution.x, 1/ScreenResolution.y);

	float3 ColorCenter = SceneTexture.Sample(ColorSampler, Input.TextureCoordinate).rgb;

	float LumaCenter = RGBToLuma(ColorCenter);

	float LumaDown = RGBToLuma(SceneTexture.Sample(ColorSampler, Input.TextureCoordinate, int2(0, -1)).rgb);
	float LumaUp = RGBToLuma(SceneTexture.Sample(ColorSampler, Input.TextureCoordinate, int2(0, 1)).rgb);
	float LumaRight = RGBToLuma(SceneTexture.Sample(ColorSampler, Input.TextureCoordinate, int2(1, 0)).rgb);
	float LumaLeft = RGBToLuma(SceneTexture.Sample(ColorSampler, Input.TextureCoordinate, int2(-1, 0)).rgb);

	float LumaMax = max(LumaCenter, max(max(LumaUp, LumaDown), max(LumaLeft, LumaRight)));
	float LumaMin = min(LumaCenter, min(min(LumaUp, LumaDown), min(LumaLeft, LumaRight)));

	float LumaRange = LumaMax - LumaMin;

	if (LumaRange < max(EDGE_THRESHOLD_MIN, EDGE_THRESHOLD_MAX * LumaMax))
	{
		OutputColor = float4(ColorCenter, 1.0f);
		return OutputColor;
	}

	float LumaDownLeft = RGBToLuma(SceneTexture.Sample(ColorSampler, Input.TextureCoordinate, int2(-1, -1)).rgb);
	float LumaDownRight = RGBToLuma(SceneTexture.Sample(ColorSampler, Input.TextureCoordinate, int2(1, -1)).rgb);
	float LumaUpLeft = RGBToLuma(SceneTexture.Sample(ColorSampler, Input.TextureCoordinate, int2(-1, 1)).rgb);
	float LumaUpRight = RGBToLuma(SceneTexture.Sample(ColorSampler, Input.TextureCoordinate, int2(1, 1)).rgb);

	float LumaUpDown = LumaUp + LumaDown;
	float LumaLeftRight = LumaLeft + LumaRight;
	float LumaLeftCorners = LumaUpLeft + LumaDownLeft;
	float LumaRightCorners = LumaUpRight + LumaDownRight;
	float LumaUpCorners = LumaUpLeft + LumaUpRight;
	float LumaDownCorners = LumaDownLeft + LumaDownRight;

	float EdgeHorizontal = abs((-2.0f*LumaLeft) + LumaLeftCorners) + abs((-2.0f*LumaCenter + LumaUpDown))*2.0f + abs((-2.0f*LumaRight) + LumaRightCorners);
	float EdgeVertical = abs((-2.0f * LumaUp) + LumaUpCorners) + abs((-2.0f * LumaCenter) + LumaLeftRight)*2.0f + abs((-2.0f * LumaDown) + LumaDownCorners);

	bool IsHorizontal = (EdgeHorizontal >= EdgeVertical);

	float Luma1 = IsHorizontal ? LumaDown : LumaLeft;
	float Luma2 = IsHorizontal ? LumaUp : LumaRight;

	float Gradient1 = Luma1 - LumaCenter;
	float Gradient2 = Luma2 - LumaCenter;

	bool Is1Steepest = (abs(Gradient1) >= (abs(Gradient2)));
	
	float GradientScaled = 0.25f*(max(abs(Gradient1), abs(Gradient2)));

	float StepLength = IsHorizontal ? InverseScreenSize.y : InverseScreenSize.x;

	float LumaLocalAverage = 0.0f;

	if (Is1Steepest)
	{
		StepLength = -StepLength;
		LumaLocalAverage = 0.5*(Luma1 + LumaCenter);
	}
	else
	{
		LumaLocalAverage = 0.5*(Luma2 + LumaCenter);
	}

	float2 CurrentUV = Input.TextureCoordinate;

	if (IsHorizontal)
	{
		CurrentUV.y += (StepLength * 0.5f);
	}
	else
	{
		CurrentUV.x += (StepLength * 0.5f);
	}

	float2 Offset = IsHorizontal ? float2(InverseScreenSize.x, 0.0f) : float2(0.0f, InverseScreenSize.y);

	float2 UV1 = CurrentUV - Offset;
	float2 UV2 = CurrentUV + Offset;

	float LumaEnd1 = RGBToLuma(SceneTexture.Sample(ColorSampler, UV1).rgb);
	float LumaEnd2 = RGBToLuma(SceneTexture.Sample(ColorSampler, UV2).rgb);
	LumaEnd1 -= LumaLocalAverage;
	LumaEnd2 -= LumaLocalAverage;

	bool Reached1 = abs(LumaEnd1) >= GradientScaled;
	bool Reached2 = abs(LumaEnd2) >= GradientScaled;
	bool ReachedBoth = Reached1 && Reached2;

	if (!Reached1)
	{
		UV1 -= Offset;
	}
	if (!Reached2)
	{
		UV2 += Offset;
	}

	if (!ReachedBoth)
	{
		for (int i = 2; i < ITERATIONS; ++i)
		{
			if (!Reached1)
			{
				LumaEnd1 = RGBToLuma(SceneTexture.Sample(ColorSampler, UV1).rgb);
				LumaEnd1 = LumaEnd1 - LumaLocalAverage;
			}

			if (!Reached2)
			{
				LumaEnd2 = RGBToLuma(SceneTexture.Sample(ColorSampler, UV2).rgb);
				LumaEnd2 = LumaEnd2 - LumaLocalAverage;
			}

			Reached1 = abs(LumaEnd1) >= GradientScaled;
			Reached2 = abs(LumaEnd2) >= GradientScaled;
			ReachedBoth = Reached1 && Reached2;

			if (!Reached1)
			{
				UV1 -= Offset * QUALITY(i);
			}
			if (!Reached2)
			{
				UV2 += Offset * QUALITY(i);
			}

			if (ReachedBoth)
			{
				break;
			}
		}
	}

	float Distance1 = IsHorizontal ? (Input.TextureCoordinate.x - UV1.x) : (Input.TextureCoordinate.y - UV1.y);
	float Distance2 = IsHorizontal ? (UV2.x - Input.TextureCoordinate.x) : (UV1.y - Input.TextureCoordinate.y);

	bool IsDirection1 = Distance1 < Distance2;
	float DistanceFinal = min(Distance1, Distance2);

	float EdgeThickness = Distance1 + Distance2;

	float PixelOffset = (-DistanceFinal / EdgeThickness) + 0.5f;

	bool IsLumaCenterSmaller = LumaCenter < LumaLocalAverage;

	bool CorrectVariation = ((IsDirection1 ? Luma1 : Luma2) < 0.0f) != IsLumaCenterSmaller;
	float FinalOffset = CorrectVariation ? PixelOffset : 0.0f;

	float LumaAverage = (1.0f / 12.0f) * (2.0f * (LumaUpDown + LumaLeftRight) + LumaLeftCorners + LumaRightCorners);
	
	float SubPixelOffset1 = clamp(abs(LumaAverage - LumaCenter) / LumaRange, 0.0f, 1.0f);
	float SubPixelOffset2 = (-2.0f * SubPixelOffset1 + 3.0f) * SubPixelOffset1 *SubPixelOffset1;

	float SubPixelOffsetFinal = SubPixelOffset2 * SubPixelOffset2 * SUBPIXEL_QUALITY;

	FinalOffset = max(FinalOffset, SubPixelOffsetFinal);

	float2 FinalUV = Input.TextureCoordinate;

	if (IsHorizontal)
	{
		FinalUV.y += FinalOffset * StepLength;
	}
	else
	{
		FinalUV.x += FinalOffset * StepLength;
	}

	OutputColor = float4(SceneTexture.Sample(ColorSampler, FinalUV).rgb, 1.0f);

	return OutputColor;*/

	float2 TexCoordOffset = float2(1 / ScreenResolution.x, 1 / ScreenResolution.y);

	float LumaTopLeft = RGBToLuma((SceneTexture.Sample(ColorSampler, Input.TextureCoordinate.xy + (float2(-1.0f, -1.0f) * TexCoordOffset)).rgb));
	float LumaTopRight = RGBToLuma((SceneTexture.Sample(ColorSampler, Input.TextureCoordinate.xy + (float2(1.0f, -1.0f) * TexCoordOffset)).rgb));
	float LumaBottomLeft = RGBToLuma((SceneTexture.Sample(ColorSampler, Input.TextureCoordinate.xy + (float2(-1.0f, 1.0f) * TexCoordOffset)).rgb));
	float LumaBottomRight = RGBToLuma((SceneTexture.Sample(ColorSampler, Input.TextureCoordinate.xy + (float2(1.0f, 1.0f) * TexCoordOffset)).rgb));
	float LumaCenter = RGBToLuma((SceneTexture.Sample(ColorSampler, Input.TextureCoordinate.xy).rgb));

	float LumaMin = min(LumaCenter, min(min(LumaTopLeft, LumaTopRight), min(LumaBottomLeft, LumaBottomRight)));
	float LumaMax = max(LumaCenter, max(max(LumaTopLeft, LumaTopRight), max(LumaBottomLeft, LumaBottomRight)));

	float2 Direction;
	Direction.x = -((LumaTopLeft + LumaTopRight) - (LumaBottomLeft + LumaBottomRight));
	Direction.y = ((LumaTopLeft + LumaBottomLeft) - (LumaTopRight + LumaBottomRight));

	float DirectionReduce = max(((LumaTopLeft + LumaTopRight + LumaBottomRight + LumaBottomLeft) * (0.25 * EDGE_THRESHOLD_MAX)), EDGE_THRESHOLD_MIN);

	float InverseDirectionAdjustment = 1.0f / (min(abs(Direction.x), abs(Direction.y)) + DirectionReduce);

	Direction = min(float2(ITERATIONS, ITERATIONS), max(float2(-ITERATIONS, -ITERATIONS), Direction * InverseDirectionAdjustment)) * TexCoordOffset;

	float3 Result1 = 0.5 * (SceneTexture.Sample(ColorSampler, Input.TextureCoordinate.xy + (Direction * (1.0 / 3.0 - 0.5f))).xyz
					+ SceneTexture.Sample(ColorSampler, Input.TextureCoordinate.xy + (Direction * (2.0 / 3.0 - 0.5))).xyz);

	float3 Result2 = Result1 * 0.5 + 0.25*(SceneTexture.Sample(ColorSampler, Input.TextureCoordinate.xy + (Direction * (0.0 / 3.0 - 0.5f))).xyz
									+ SceneTexture.Sample(ColorSampler, Input.TextureCoordinate.xy + (Direction * (3.0 / 3.0 - 0.5))).xyz);

	float LumaResult2 = RGBToLuma(Result2);

	bool finalResult = LumaResult2 < LumaMin || LumaResult2 > LumaMax;

	OutputColor = lerp(float4(Result1, 1.0f), float4(Result2, 1.0f), finalResult);

	//if (LumaResult2 < LumaMin || LumaResult2 > LumaMax)
	//{
	//	OutputColor = float4(Result1, 1.0);
	//}
	//else
	//{
	//	OutputColor = float4(Result2, 1.0);
	//}

	return OutputColor;
}