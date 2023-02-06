struct VSInput
{
    float3 position     : POSITION0;
    float3 normal       : NORMAL0;
    float3 tangent      : TANGENT0;
    float3 bitangent    : BITANGENT0;
    float4 UV           : TEXCOORD0;
};
struct VSOutput
{
    float4 outPosition  : SV_POSITION;
    float4 wsPosition   : WSPOSITION0;
    float3 outNormal    : NORMAL0;
    float3 outTangent      : TANGENT0;
    float3 outBitangent    : BITANGENT0;
    float2 outUV        : TEXCOORD0;
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


    output.outNormal = mul(input.normal, (float3x3)WorldMatrix);
    //output.outNormal = normalize(output.outNormal);

    output.outTangent = mul(input.tangent, (float3x3)WorldMatrix);
    //output.outTangent = normalize(output.outTangent);

    output.outBitangent = mul(input.bitangent, (float3x3)WorldMatrix);
    //output.outBitangent = normalize(output.outBitangent);
    
    output.outUV = input.UV;
	return output;
}