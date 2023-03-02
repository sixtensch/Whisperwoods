#include "Constants.hlsli"
#include "TimeSwitchFuncs.hlsli"

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

// Based on http://www.oscars.org/science-technology/sci-tech-projects/aces
float3 AcesTonemap(float3 color)
{
    float3x3 m1 = float3x3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
	);
    
    float3x3 m2 = float3x3(
        1.60475, -0.10208, -0.00327,
        -0.53108, 1.10813, -0.07276,
        -0.07367, -0.00605, 1.07602
	);
    float3 v = mul(color, m1);
    float3 a = v * (v + 0.0245786) - 0.000090537;
    float3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    
    float3 sRGBSpace = mul((a / b), m2); // Transform HDR to UNORM sRGB space.
    return saturate(sRGBSpace);
}

cbuffer COLORGRADE_INFO_BUFFER : REGISTER_CBV_COLORGRADE_INFO
{
    float2 vignette; // x: Inner border radius, y: Vignette strength
    float2 contrast; // x: Contrast amount, y: Midpoint value
    float brightness; // Brightness offset
    float saturation; // Saturation value
};

cbuffer TIME_SWITCH_INFO_BUFFER : REGISTER_CBV_SWITCH_INFO
{
    float timeSinceSwitch;
    float timeSwitchStartDuration;
    float timeSwitchEndDuration;
    bool isInFuture;
    
    float detectionLevel;
    float PADDING[3];
}

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D renderTexture : REGISTER_SRV_COPY_SOURCE;
Texture2D lumSumTexture : REGISTER_SRV_TEX_USER_4;

float4 main(PS_INPUT input) : SV_TARGET
{
    const float totalTimeSwitchInfluence =
        TotalTimeSwitchInfluence(
        timeSinceSwitch,
        timeSwitchStartDuration,
        timeSwitchEndDuration
    );
    
    uint3 texPos = uint3(input.uv.x * WINDOW_WIDTH, input.uv.y * WINDOW_HEIGHT, 0u);
    
    float3 color = renderTexture.Load(texPos).rgb + lumSumTexture.Load(texPos).rgb;
    color = AcesTonemap(color);
    
    //color = Tint(color, lerp(1.0f.rrr, float3(0.0f, 0.0f, 2.0f), totalInflunce));
    //color = Brightness(color, lerp(brightness, 1.0f, totalInflunce));
    color = Saturation(color, smoothstep(saturation, 0.0f, totalTimeSwitchInfluence));
    //color = Saturation(color, saturation);
    color = Contrast(color, contrast.x, contrast.y);
    
    color = Vignette(color, input.uv, lerp(vignette.x, vignette.x * 3.0f, detectionLevel), vignette.y);
    
    //color = pow(color, (1.0f / 2.2f)); // Gamma correction.
    return float4(color, 1.0f);
}