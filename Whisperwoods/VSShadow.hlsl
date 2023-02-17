#include "Constants.hlsli"

cbuffer ViewInfo : REGISTER_CBV_VIEW_INFO
{
    matrix ViewMatrix;
    matrix ProjectionMatrix;
};
cbuffer ObjectInfo : REGISTER_CBV_OBJECT_INFO
{
    matrix WorldMatrix;
};

float4 main( float3 pos : POSITION ) : SV_POSITION
{
    matrix mvpMatrix = mul(mul(WorldMatrix, ViewMatrix), ProjectionMatrix);
    return mul(float4(pos,1.0f), mvpMatrix);
}