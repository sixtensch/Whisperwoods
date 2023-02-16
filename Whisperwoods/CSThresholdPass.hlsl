#include "Constants.hlsli"
#include "ComputeConstants.hlsli"

Texture2D<float4> renderTexture         : REGISTER_SRV_COPY_SOURCE;
RWTexture2D<float4> lumTexture          : REGISTER_UAV_SYSTEM_0;

// sRGB luminance filtering.
//float3 FilterBrightness(float3 color, float thresholdValue, float strength)
//{
//    float3 properGrayscale = float3(0.2126f, 0.7152f, 0.0722f);
//    float luminance = dot(color, properGrayscale);
//
//    return smoothstep(thresholdValue, 1.0f, luminance) * color * strength;
//}

// HDR luminance filtering.
float3 FilterBrightness(float3 color, float thresholdValue, float strength)
{
    color = clamp(color, 0.0f, thresholdValue);
    float brightness = max(max(color.r, color.g), color.b); // Maximum brightness
    
    float minValOffset = 0.1f; // Any light that is below this min value is pushed to 0.
    float luminance = smoothstep(0.0f, thresholdValue - minValOffset, brightness - minValOffset); // Scalar [0, 1] on how close to max brightness pixel is.

    return color * luminance * strength;
}

[numthreads(NUM_THREADS.x, NUM_THREADS.y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint3 texPos = uint3(DTid.xy, 0);
    
    float3 texColor = renderTexture.Load(texPos).rgb;
    texColor = FilterBrightness(texColor, 5.0f, 1.0f);
    
    lumTexture[texPos.xy] = float4(texColor, 1.0f);
}