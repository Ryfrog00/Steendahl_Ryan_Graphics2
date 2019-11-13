// rule of three
// Three things must match
// 1. C++ Vertex STruct
// 2. Input Layout
// 3. HLSL Vertex Struct


struct InputVertex
{
	float3 xyz : POSITION;
	float3 uvw : TEXCOORD;
	float3 nrm : NORMAL;
};

struct OutputVertex
{
	float4 xyzw : SV_POSITION; //system value
	float4 rgba : COLOR;
	float3 uv : UV;
	float3 nrm : NORM;
	float4 posL : POS_FOR_LIGHT;
};

cbuffer SHADER_VARS : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

OutputVertex main(InputVertex input)
{
	

	OutputVertex output = (OutputVertex)0;
	output.xyzw = float4(input.xyz, 1);
	output.posL = float4(input.xyz, 1);
	output.nrm = input.nrm;
	output.uv = input.uvw;
	//output.rgba.rgb = input.nrm;
	// Do math here (shader intrinsics)
	output.xyzw = mul(worldMatrix, output.xyzw);
	output.xyzw = mul(viewMatrix, output.xyzw);
	output.xyzw = mul(projectionMatrix, output.xyzw);


	// don't do perspective divide

	return output;
}