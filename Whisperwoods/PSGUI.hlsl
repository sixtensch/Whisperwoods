struct VSOutput
{
	float4 outPosition	: SV_POSITION;
	float4 wPosition    : WPOSITION0;
	float3 outNormal	: NORMAL0;
	float2 outUV		: TEXCOORD0;
};

float4 main() : SV_TARGET0
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}