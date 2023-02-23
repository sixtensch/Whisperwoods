#include "Constants.hlsli"
#include "ComputeConstants.hlsli"

Texture2D<float4> luminanceTexture                  : REGISTER_SRV_TEX_USER_0;
RWTexture2D<float4> targetTexture                   : REGISTER_UAV_SYSTEM_0;

SamplerState bloomSampler                           : REGISTER_SAMPLER_SYSTEM_0;



groupshared float4 sharedData[COMPUTE_THREADS_X + 2u][COMPUTE_THREADS_Y + 2u];

[numthreads(COMPUTE_THREADS_X, COMPUTE_THREADS_Y, 1)]
void main( uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
    uint2 targetTextureDims;
    targetTexture.GetDimensions(targetTextureDims.x, targetTextureDims.y);
    
    float2 texUV = DTid.xy / float2(targetTextureDims);
    //texUV += (1.0f / targetTextureDims) * 0.5f; // Adjust to middle of texel.
    
    uint2 sharedMemPos = GTid.xy + uint2(1u, 1u);
    sharedData[sharedMemPos.x - 1u][sharedMemPos.y - 1u] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //sharedData[sharedMemPos.x + 1u][sharedMemPos.y + 1u] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    AllMemoryBarrierWithGroupSync();
    

    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    for (int mipLevel = BLOOM_MIP_LEVELS - 1; mipLevel >= 0; mipLevel--)
    {
        //uint mipWidth;
        //uint mipHeight;
        //uint numberOfLevels;
        //luminanceTexture.GetDimensions(mipLevel, mipWidth, mipHeight, numberOfLevels);
        
        //float2 mipTexelSize = 1.0f / float2(mipWidth, mipHeight);
        
        // Clever way of offsetting texels.
        //float sampleRadius = 1.0f;
        //float4 offset = mipTexelSize.xyxy * float4(1.0f, 1.0f, -1.0f, 0.0f) * sampleRadius;
        
 
        float4 mipLevelColor = luminanceTexture.SampleLevel(bloomSampler, texUV, mipLevel) * 4.0f;
        sharedData[sharedMemPos.x][sharedMemPos.y] = mipLevelColor;
        AllMemoryBarrierWithGroupSync();
        
        // Bottom row
        mipLevelColor += sharedData[sharedMemPos.x - 1u][sharedMemPos.y + 1u];
        mipLevelColor += sharedData[sharedMemPos.x     ][sharedMemPos.y + 1u] * 2.0f;
        mipLevelColor += sharedData[sharedMemPos.x + 1u][sharedMemPos.y + 1u];
        
        // Middle row, skip center
        mipLevelColor += sharedData[sharedMemPos.x - 1u][sharedMemPos.y     ] * 2.0f;
        mipLevelColor += sharedData[sharedMemPos.x + 1u][sharedMemPos.y     ] * 2.0f;
        
        // Top row
        mipLevelColor += sharedData[sharedMemPos.x - 1u][sharedMemPos.y - 1u];
        mipLevelColor += sharedData[sharedMemPos.x     ][sharedMemPos.y - 1u] * 2.0f;
        mipLevelColor += sharedData[sharedMemPos.x + 1u][sharedMemPos.y - 1u];
        
        //// Bottom row
        //mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV - offset.xy, mipLevel);
        //mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV - offset.wy, mipLevel) * 2.0f;
        //mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV - offset.zy, mipLevel);
        //
        //// Middle row, skip center
        //mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV + offset.zw, mipLevel) * 2.0f;
        //mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV + offset.xy, mipLevel) * 2.0f;
        //
        //// Top row
        //mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV + offset.zy, mipLevel);
        //mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV + offset.wy, mipLevel) * 2.0f;
        //mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV + offset.xy, mipLevel);
        
        // Take account for lower (blurrier) mips a bit more.
        float mipLevelBias = log(mipLevel + 2) * 1.1f; 
        float tentFilterAdjustment = 1.0f;
        mipLevelColor *= (1.0f / 16.0f) * tentFilterAdjustment * mipLevelBias;
        
        finalColor += mipLevelColor;
    }
    
    targetTexture[DTid.xy] = float4(finalColor, 1.0f);
}