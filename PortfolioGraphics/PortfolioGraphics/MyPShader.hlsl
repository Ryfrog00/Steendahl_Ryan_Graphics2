
texture2D baseTexture : register(t0); 

SamplerState filter: register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(float4 posH : SV_POSITION, float4 icolor : COLOR, float3 uvs : UV, float3 nrm : NORM) : SV_TARGET
{
	float3 lightDir = {-1.0f, 0.5f, 0.5f};
	float LIGHTRATIO = clamp(dot(lightDir, nrm), 0, 1);
	float4 lightClr = { 1, 0, 0, 1 };
	float4 RESULT = LIGHTRATIO * lightClr * baseTexture.Sample(filter, uvs);
	float3 lightDir2 = { 0.5f, -1.0f, 0.7f };
	float LIGHTRATIO2 = clamp(dot(lightDir2, nrm), 0, 1);
	float4 lightClr2 = { 0, 0, 1, 1 };
	RESULT = ((LIGHTRATIO2 * lightClr2) + (LIGHTRATIO * lightClr)) * baseTexture.Sample(filter, uvs);
	return RESULT;
}

