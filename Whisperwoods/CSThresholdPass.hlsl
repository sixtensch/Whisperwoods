#include "Constants.hlsli"
#include "ComputeConstants.hlsli"

RWTexture2D<unorm float4> backBufferTexture : REGISTER_UAV_RENDER_TARGET;
RWTexture2D<unorm float4> outTexture        : REGISTER_UAV_DEFAULT;

float3 FilterLuminance(float3 color, float thresholdValue, float strength)
{
    float3 properGrayscale = float3(0.2126f, 0.7152f, 0.0722f);
    float luminance = dot(color, properGrayscale);

    return smoothstep(thresholdValue, 1.0f, luminance) * color * strength;
}

[numthreads(NUM_THREADS.x, NUM_THREADS.y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint3 texPos = uint3(DTid.xy, 0);
    
    float3 texColor = backBufferTexture.Load(texPos).rgb;
    texColor = FilterLuminance(texColor, 0.8f, 1.0f);
    
    outTexture[texPos.xy] = float4(texColor, 1.0f);
}