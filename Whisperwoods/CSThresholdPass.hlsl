#include "Constants.hlsli"
#include "ComputeConstants.hlsli"
#include "TimeSwitchFuncs.hlsli"

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

cbuffer THRESHOLD_INFO_BUFFER : REGISTER_CBV_THRESHOLD_INFO
{
    float luminanceThreshold;
    float strength;
    float minLuminance;
};

cbuffer TIME_SWITCH_INFO_BUFFER : REGISTER_CBV_SWITCH_INFO
{
    float timeSinceSwitch;
    float timeSwitchStartDuration;
    float timeSwitchEndDuration;
    bool isInFuture;
    
    bool PADDING[3];
}

#define REGION_SIZE_PER_THREAD 2
[numthreads(NUM_THREADS.x, NUM_THREADS.y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint3 texPos = uint3(DTid.xy, 0);
    texPos *= REGION_SIZE_PER_THREAD;
    
    [unroll]
    for (uint y = texPos.y; y < texPos.y + REGION_SIZE_PER_THREAD; ++y)
    [unroll]
        for (uint x = texPos.x; x < texPos.x + REGION_SIZE_PER_THREAD; ++x)
        {
            float3 texColor = renderTexture.Load(texPos).rgb;
            float totalInflunce = TotalTimeSwitchInfluence(
                timeSinceSwitch,
                timeSwitchStartDuration,
                timeSwitchEndDuration
            );
            
            texColor = FilterBrightness(
                texColor,
                lerp(luminanceThreshold, 0.0f, totalInflunce),
                lerp(strength, strength * 1.4f, totalInflunce),
                lerp(minLuminance, 0.0f, totalInflunce)
            );
            
            uint2 coord = uint2(x, y);
            lumTexture[coord] = float4(texColor, 1.0f);
        }
}