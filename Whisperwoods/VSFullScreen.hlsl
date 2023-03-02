struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float2 outUV : TEXCOORD;
};

VS_OUTPUT main(uint vertID : SV_VertexID)
{
    VS_OUTPUT output;
    
    // Creates UV from vertID.
    output.outUV = float2(vertID & 1, vertID >> 1);
	
    // Scales from [0, 1] to [-1, 1] 
    output.outPos = float4((output.outUV.x - 0.5f) * 2.0f, -(output.outUV.y - 0.5f) * 2.0f, 0.0f, 1.0f);
    
	return output;
}