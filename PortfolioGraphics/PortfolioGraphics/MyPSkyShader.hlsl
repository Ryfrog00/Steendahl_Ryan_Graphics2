
SamplerState filter: register(s0); // filter 0 using CLAMP, filter 1 using WRAP
TextureCube baseTexture : register(t0); 




// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(float4 posH : SV_POSITION, float4 icolor : COLOR, float3 uvs : UV, float3 nrm : NORM, float4 posL : POS_FOR_LIGHT) : SV_TARGET
{
	
	//determining final color with lights
	return  baseTexture.Sample(filter, uvs);
	
}

