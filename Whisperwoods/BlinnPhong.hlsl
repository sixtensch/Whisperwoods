struct VSOutput
{
    float4 outPosition : SV_POSITION;
    float4 wsPosition  : WSPOSITION0;
    float3 outNormal   : NORMAL0;
    float2 outUV       : TEXCOORD0;
};


float4 main(VSOutput input) : SV_TARGET
{
	return input.wsPosition;
}