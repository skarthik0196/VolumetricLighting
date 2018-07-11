struct VS_OUTPUT
{
	float4 LightPosition : SV_POSITION;
	float4 LightPosition1 : POSITION1;
	float4 LightPosition2 : POSITION2;
	float4 LightPosition3 : POSITION3;
};

struct PS_OUTPUT
{
	float4 Depth1 : SV_TARGET0;
	float4 Depth2 : SV_TARGET1;
	float4 Depth3 : SV_TARGET2;
};

PS_OUTPUT main(VS_OUTPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT )0;
	float LightDepth1 = Input.LightPosition1.z / Input.LightPosition1.w;
	float LightDepth2 = Input.LightPosition2.z / Input.LightPosition2.w;
	float LightDepth3 = Input.LightPosition3.z / Input.LightPosition3.w;

	Output.Depth1 = float4(LightDepth1, LightDepth1, LightDepth1, 1.0f);
	Output.Depth2 = float4(LightDepth2, LightDepth2, LightDepth2, 1.0f);
	Output.Depth3 = float4(LightDepth3, LightDepth3, LightDepth3, 1.0f);
	
	return Output;
}