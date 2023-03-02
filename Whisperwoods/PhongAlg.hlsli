
float4 phong(
    float3 voxelPosition, 
    float3 voxelNormal,
    float3 lightIntensity,
    float3 lightDirection,
    float lightScalar,
    float3 cameraDirection,
    float3 materialAlbedo,
    float3 materialSpecular,
    float materialOpacity,
    float materialSpecularity)
{
    float incidence = dot(voxelNormal, lightDirection);
    float3 reflection = reflect(voxelNormal, -lightDirection);
    
    float3 diffuse = materialAlbedo * lightIntensity * max(incidence, 0.0f);

    float view = max(0.0f, dot(reflection, cameraDirection));
    float3 specular = max(materialSpecular * lightIntensity * pow(view, materialSpecularity), float3(0, 0, 0));

    return float4((diffuse + specular) * lightScalar, materialOpacity);
}