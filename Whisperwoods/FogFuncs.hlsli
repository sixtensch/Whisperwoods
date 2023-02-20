// Applies fog effect to output color. 
// Density 0 means no fog density 1 is very thicc. 
// Strength modifies influence of fog color.
// Desmos input for distance (x) testing: \left(1-\frac{1}{e^{\left(x\cdot d\right)^{2}}}\right)
float3 ApplyExpFog(float3 color, float3 playerPos, float3 worldPos, float3 fogColor, float fogDensity, float fogStrength)
{
    float distance = distance(playerPos, worldPos);
	
    float denominator = exp(pow(distance * fogDensity, 2.0f));
    float fogAmount = 1.0f - (1.0f / denominator);
	
    return lerp(color.rgb, fogColor * fogStrength, fogAmount);
}
