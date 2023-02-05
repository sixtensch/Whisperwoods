struct VSInput
{
    float3 Position : POSITION0;
    float3 normal   : NORMAL0;
    float2 UV       : TEXCOORD0;
};
struct VSOutput
{
    float4 outPosition : SV_POSITION;
    float4 wsPosition  : WSPOSITION0;
    float3 outNormal   : NORMAL0;
    float2 outUV       : TEXCOORD0;
};

cbuffer VSData
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjectionMatrix;
};

VSOutput main(VSInput input)
{
    VSOutput output;
	
    matrix wvMatrix = mul(WorldMatrix, ViewMatrix);
    matrix wvpMatrix = mul(wvMatrix, ProjectionMatrix);
    
    //output.wsPosition = mul(float4(input.Position, 1.0f), wvMatrix);
    //output.outPosition = mul(float4(input.Position, 1.0f), wvpMatrix);
    
    // For the DUD
    output.outPosition = float4(input.Position, 1.0f);
    output.wsPosition = float4(input.Position, 1.0f);
    
    output.outNormal = input.normal;
    output.outUV = input.UV;
	return output;
}