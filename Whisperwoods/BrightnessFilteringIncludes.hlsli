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
