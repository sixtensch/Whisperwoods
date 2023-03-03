#include "Constants.hlsli"
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

SamplerState linearSampler : REGISTER_SAMPLER_BORDER_LINEAR;

Texture2D<float4> renderTexture : REGISTER_SRV_COPY_SOURCE;
RWTexture2D<float4> lumTexture : REGISTER_UAV_SYSTEM_0;
RWTexture2D<float4> lumTextureCopy : REGISTER_UAV_SYSTEM_1;

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

[numthreads(8u, 8u, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint3 texPos = uint3(DTid.xy, 0u);
    
    uint outputTexWidth = 0u;
    uint outputTexHeight = 0u;
    lumTexture.GetDimensions(outputTexWidth, outputTexHeight);
    
    float2 texelSize = 1.0f.xx / float2(outputTexWidth, outputTexHeight);
    float2 textureCoord = texPos.xy / float2(outputTexWidth, outputTexHeight) + texelSize / 2.0f;
    
    float3 texColor = renderTexture.SampleLevel(linearSampler, textureCoord, 0).rgb;

    float totalInflunce =
        TotalTimeSwitchInfluence(
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
    
    lumTexture[texPos.xy] = float4(texColor, 1.0f);
    lumTextureCopy[texPos.xy] = float4(texColor, 1.0f);
}