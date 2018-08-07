struct VS_INPUT
{
	float4 Position : POSITION;
	float2 TextureCoordinate : TEXTURECOORDINATE;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinate : TEXCOORD;
};

VS_OUTPUT main(uint VertexID : SV_VertexID /*VS_INPUT Input*/)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	Output.TextureCoordinate = float2((VertexID << 1) & 2, VertexID & 2);
	Output.Position = float4(Output.TextureCoordinate * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	//Output.Position = Input.Position;
	//Output.TextureCoordinate = Input.TextureCoordinate;

	return Output;
}