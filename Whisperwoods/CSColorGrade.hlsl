#include "Constants.hlsli"
#include "ComputeConstants.hlsli"

struct VSOutput
{
    float4 outPosition : SV_POSITION;
    float2 outUV : TEXCOORD0;
};

float3 Tint(float3 color, float3 tintColor)
{
    float3 finalCol = color * tintColor;
    
    return saturate(finalCol);
}

float3 Brightness(float3 color, float3 brightness)
{
    return saturate(color + brightness);
}

float3 Brightness(float3 color, float brightness)
{
    return saturate(color + brightness.rrr);
}

float3 Saturation(float3 color, float saturationAmount)
{
    float3 properGrayScale = float3(0.2126f, 0.7152f, 0.0722f);
    float luminance = dot(color, properGrayScale);
    
    return saturate(lerp(luminance.rrr, color, saturationAmount));
}

float3 Contrast(float3 color, float contrastAmount, float midpoint)
{
    float3 sg = sign(color - midpoint);
    
    color = sg * pow(
        abs(color - midpoint) * 2.0f,
        (1.0f / contrastAmount).rrr
    );
    
    return color * 0.5f + midpoint;
}

float3 Vignette(float3 color, float2 uvs, float innerBorderRadius, float strength)
{
    float vignetteAmount = smoothstep(0.70710f, 1.0f - innerBorderRadius, distance(float2(0.5f, 0.5f), uvs));
    return color * pow(vignetteAmount, strength);
}


RWTexture2D<unorm float4> backBufferTexture : REGISTER_UAV_RENDER_TARGET;

[numthreads(NUM_THREADS.x, NUM_THREADS.y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{   
    const uint3 texPos = uint3(DTid.xy, 0u);
    const float2 texUV = float2(texPos.xy) / BACK_BUFFER_RESOLUTION;
    
    float3 color = backBufferTexture.Load(texPos).rgb;
    
    color = Tint(color, float3(1.0f, 1.0f, 1.0f));
    color = Brightness(color, -0.02f);
    color = Saturation(color, 1.4f);
    //color = Contrast(color, 2.0f, 0.4f);
    
    float2 pixelCoords = float2(texPos.xy);
    float2 circleCenter = BACK_BUFFER_RESOLUTION * 0.5f;
    float circleRadius = BACK_BUFFER_RESOLUTION.y * 0.5f * 1.0f;
    float circleThickness = 30.0f;
    float signedDistance = length(pixelCoords - circleCenter) - circleRadius;
    float glowAmount = smoothstep(0.0f, circleThickness, abs(signedDistance));
    glowAmount = 1.0f - pow(glowAmount, 0.125f);
    
    float3 glowColor = float3(0.1f, 0.6f, 0.8f);
    color += lerp(0.0f, glowColor, glowAmount);
    
    color = Vignette(color, texUV, 0.5f, 5.0f);
    
    backBufferTexture[texPos.xy] = float4(color, 1.0f);
}