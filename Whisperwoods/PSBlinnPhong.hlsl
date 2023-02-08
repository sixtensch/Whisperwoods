
struct VSOutput
{
    float4 outPosition : SV_POSITION;
    float4 wsPosition  : WSPOSITION0;
    float3 outNormal   : NORMAL0;
    float3 outTangent     : TANGENT0;
    float3 outBitangent   : BITANGENT0;
    float2 outUV       : TEXCOORD0;
};


float4 main(VSOutput input) : SV_TARGET
{
    float3 normalColor = input.outNormal;
    float3x3 TBN = float3x3(input.outTangent, input.outBitangent, input.outNormal);
    // make normal in range -1 to 1
    normalColor = (2.0f * normalColor) - 1.0f;
    return  float4(mul(normalColor, TBN), 1.0);
	//return float4(input.outTangent,1);
}