
texture2D baseTexture : register(t0); 

SamplerState filter: register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(float4 posH : SV_POSITION, float4 icolor : COLOR, float3 uvs : UV, float3 nrm : NORM, float4 posL : POS_FOR_LIGHT) : SV_TARGET
{
	//point light
	float4 pointLightPosition = {-5.0f, 3.8f, -5.0f, 1.0f};
	float4 pointLightColor = { 1, 1, 0, 1 };
	float3 toNormalize = pointLightPosition - posL;
	float3 pointLightDir = normalize(toNormalize);
	float pointLightRatio = clamp(dot(pointLightDir, nrm), 0, 1);
	float4 pointLight = pointLightRatio * pointLightColor;
	float pointAttenuation = (1.0f - clamp((length(toNormalize) / 35.0f), 0, 1));

	float4 spotLightPosition = { 0.0f, 10.0f, 0.0f, 1.0f };
	float3 coneDir = { 0.2f, -1.0f, 0.2f };
	float3 spotToNormalize = spotLightPosition - posL;
	float3 spotLightDir = normalize(spotToNormalize);
	float spotSurfaceRatio = clamp(dot((-1.0f*spotLightDir), coneDir), 0, 1);
	float spotFactor = (spotSurfaceRatio > 0.25) ? 1 : 0;
	float spotLightRatio = clamp(dot(spotLightDir, nrm), 0, 1);
	float innerConeRatio = 0.75f;
	float outterConeRatio = 0.25f;
	float spotAttenuation = 1.0f - clamp(((innerConeRatio - spotSurfaceRatio) / (innerConeRatio - outterConeRatio)), 0, 1);
	float4 spotLightColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	

	//directional lights
	float3 lightDir = {-1.0f, 0.5f, 0.5f};
	float LIGHTRATIO = clamp(dot(lightDir, nrm), 0, 1);
	float4 lightClr = { 1, 0, 0, 1 };

	//determining final color with lights
	float4 RESULT = ((LIGHTRATIO * lightClr) + (pointLight*(pointAttenuation*pointAttenuation))+ ((spotFactor * spotLightRatio * spotLightColor) * (spotAttenuation * spotAttenuation))) * baseTexture.Sample(filter, uvs);
	return RESULT;
}

