struct VSInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent   : BITANGENT;
    float4 UV       : TEXCOORD;
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
    
    output.wsPosition = mul(float4(input.position, 1.0f), wvMatrix);
    output.outPosition = mul(float4(input.position, 1.0f), wvpMatrix);
    
    output.outNormal = input.normal;
    output.outUV = input.UV;
	return output;
}