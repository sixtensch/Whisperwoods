#include "Constants.hlsli"
#include "ComputeConstants.hlsli"

// HDR luminance filtering.
float3 FilterBrightness(float3 color, float thresholdValue, float strength, float minLuminance)
{
    color = clamp(color, 0.0f, thresholdValue);
    float brightness = max(max(color.r, color.g), color.b); // Maximum brightness
    
    float minValOffset = minLuminance; // Any light that is below this min value is pushed to 0.
    float luminance = smoothstep(0.0f, thresholdValue - minValOffset, brightness - minValOffset); // Scalar [0, 1] on how close to max brightness pixel is.

    return color * luminance * strength;
}

Texture2D<float4> renderTexture : REGISTER_SRV_COPY_SOURCE;
RWTexture2D<float4> lumTexture : REGISTER_UAV_SYSTEM_0;

cbuffer THRESHOLD_INFO_BUFFER : REGISTER_CBV_USER_0
{
    float luminanceThreshold;
    float strength;
    float minLuminance;

    float PADDING;
};

[numthreads(NUM_THREADS.x, NUM_THREADS.y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint3 texPos = uint3(DTid.xy, 0);
    
    float3 texColor = renderTexture.Load(texPos).rgb;
    texColor = FilterBrightness(texColor, luminanceThreshold, strength, minLuminance);
    
    lumTexture[texPos.xy] = float4(texColor, 1.0f);
}