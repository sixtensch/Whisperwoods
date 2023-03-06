
float4 phongLite(
    //float3 voxelPosition, 
    //float3 voxelNormal,
    float3 lightIntensity,
    //float3 lightDirection,
    float lightScalar,
    //float3 cameraDirection,
    float3 materialAlbedo
    //float3 materialSpecular,
    //float materialSpecularity
)
{
    //float incidence = dot(voxelNormal, lightDirection);
    //float3 reflection = 2 * voxelNormal * incidence - lightDirection;
    
    float3 diffuse = materialAlbedo * lightIntensity /** max(incidence, 0.0f)*/;

    //float view = max(0.0f, dot(reflection, cameraDirection));
    //float3 specular = max(materialSpecular * lightIntensity * pow(view, materialSpecularity), float3(0, 0, 0));

    return float4((diffuse) * lightScalar, 1.0f);
}