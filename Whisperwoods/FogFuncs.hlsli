// Applies fog effect to output color. 
// Density 0 means no fog density 1 is very thicc. 
// Strength modifies influence of fog color.
// Desmos input for distance (x) testing: \left(1-\frac{1}{e^{\left(x\cdot d\right)^{2}}}\right)
float3 ApplyExpFog(float3 color, float density, float distance, float3 fogColor, float fogStrength)
{
    float fogFactor = 1.0f / exp(pow(distance * density, 2.0f));
    fogFactor = 1.0f - fogFactor;
    
    return lerp(color, fogColor * fogStrength, fogFactor);
}
