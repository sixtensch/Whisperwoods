#include "Constants.hlsli"
#include "ComputeConstants.hlsli"

Texture2D<float4> luminanceTexture                  : REGISTER_SRV_TEX_USER_0;
RWTexture2D<float4> targetTexture                   : REGISTER_UAV_SYSTEM_0;

SamplerState bloomSampler                           : REGISTER_SAMPLER_BORDER_LINEAR;


[numthreads(NUM_THREADS.x, NUM_THREADS.y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint2 backBufferDims;
    targetTexture.GetDimensions(backBufferDims.x, backBufferDims.y);
    
    float2 texUV = DTid.xy / float2(backBufferDims);
    texUV += (1.0f / backBufferDims) * 0.5f; // Adjust to middle of texel.
    
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    for (int mipLevel = BLOOM_MIP_LEVELS - 1; mipLevel >= 0; mipLevel--)
    {
        uint mipWidth;
        uint mipHeight;
        uint numberOfLevels;
        luminanceTexture.GetDimensions(mipLevel, mipWidth, mipHeight, numberOfLevels);
        
        float2 mipTexelSize = 1.0f / float2(mipWidth, mipHeight);
        
        // Clever way of offsetting texels.
        float sampleRadius = 1.0f;
        float4 offset = mipTexelSize.xyxy * float4(1.0f, 1.0f, -1.0f, 0.0f) * sampleRadius;
        
        
        float4 mipLevelColor = luminanceTexture.SampleLevel(bloomSampler, texUV, mipLevel) * 4.0f;
        
        // Bottom row
        mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV - offset.xy, mipLevel);
        mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV - offset.wy, mipLevel) * 2.0f;
        mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV - offset.zy, mipLevel);
        
        // Middle row, skip center
        mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV + offset.zw, mipLevel) * 2.0f;
        mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV + offset.xy, mipLevel) * 2.0f;
        
        // Top row
        mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV + offset.zy, mipLevel);
        mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV + offset.wy, mipLevel) * 2.0f;
        mipLevelColor += luminanceTexture.SampleLevel(bloomSampler, texUV + offset.xy, mipLevel);
        
        float mipLevelBias = log(mipLevel + 2) * 1.1f; // Take account for lower (blurrier) mips a bit more.
        float tentFilterAdjustment = 0.5f;
        mipLevelColor *= (1.0f / 16.0f) * tentFilterAdjustment * mipLevelBias;
        
        finalColor += mipLevelColor.rgb;
    }
    
    targetTexture[DTid.xy] = float4(finalColor, 1.0f);
}