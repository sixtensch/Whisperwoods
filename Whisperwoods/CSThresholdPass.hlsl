#include "Constants.hlsli"
#include "ComputeConstants.hlsli"

// Any color that has a value over this will be clamped.
#define MAXIMUM_COLOR_BRIGHTNESS 25.0f

// HDR luminance filtering.
float3 FilterBrightness(float3 color, float thresholdValue, float strength, float minLuminance)
{
    color = clamp(color, 0.0f, MAXIMUM_COLOR_BRIGHTNESS);
    float brightness = max(max(color.r, color.g), color.b); // Maximum brightness
    float luminance = smoothstep(minLuminance, thresholdValue, brightness); // Scalar [0, 1] on how close to max brightness pixel is.

    return color * luminance * strength;
}

Texture2D<float4> renderTexture : REGISTER_SRV_COPY_SOURCE;
RWTexture2D<float4> lumTexture : REGISTER_UAV_SYSTEM_0;

cbuffer THRESHOLD_INFO_BUFFER : REGISTER_CBV_USER_0
{
    float luminanceThreshold;
    float strength;
    float minLuminance;
    float time;
};

[numthreads(NUM_THREADS.x, NUM_THREADS.y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint3 texPos = uint3(DTid.xy, 0);
    
    float3 texColor = renderTexture.Load(texPos).rgb;
    float effectDuration = .5f;
    float effectEndDutation = 1.5f;
    float switchInfluence = smoothstep(0.0f, effectDuration, time); // Goes to 1.
    float switchEndInfluence = 1.0f - smoothstep(effectDuration, effectDuration + effectEndDutation, time); // Goes to 1.
    float totalInflunce = switchInfluence * switchEndInfluence;
    
    
    texColor = FilterBrightness(
        texColor, 
        lerp(luminanceThreshold, 0.01f, totalInflunce),
        lerp(strength, strength * 1.4f, totalInflunce),
        lerp(minLuminance, 0.0f, totalInflunce)
    );
    
    lumTexture[texPos.xy] = float4(texColor, 1.0f);
}