// Applies fog effect to output color. 
// Density 0 means no fog density 1 is very thicc. 
// Strength modifies influence of fog color.
// Desmos input for distance (x) testing: \left(1-\frac{1}{e^{\left(x\cdot d\right)^{2}}}\right)

// Used in pixel shader for inserting into apply fog function.
static const float3 STATE_FOG_COLORS[2] = { float3(0.02f, 0.04f, 0.05f), float3(0.70f, 0.50f, 0.3f) };
static const float STATE_FOG_STRENGTHS[2] = { 1.0f, 1.7f };
static const float STATE_FOG_DENSITIES[2] = { 0.06f, 0.15f };

float3 ApplyExpFog(float3 color, float density, float distance, float3 fogColor, float fogStrength)
{
    float fogFactor = 1.0f / exp(pow(distance * density, 2.0f));
    fogFactor = 1.0f - fogFactor;
    
    return lerp(color, fogColor * fogStrength, fogFactor);
}
