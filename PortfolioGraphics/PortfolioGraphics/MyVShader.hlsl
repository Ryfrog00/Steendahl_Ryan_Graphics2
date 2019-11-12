// rule of three
// Three things must match
// 1. C++ Vertex STruct
// 2. Input Layout
// 3. HLSL Vertex Struct


struct InputVertex
{
	float4 xyzw : POSITION;
	float4 rgba : COLOR;
};

struct OutputVertex
{
	float4 xyzw : SV_POSITION; //system value
	float4 rgba : COLOR;
	float3 uvs : UV;
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
	output.xyzw = input.xyzw;
	output.rgba = input.rgba;
	output.uvs = input.xyzw;
	output.nrm = 0;
	output.posL = 0;
	// Do math here (shader intrinsics)

	output.xyzw = mul(worldMatrix, output.xyzw);
	output.xyzw = mul(viewMatrix, output.xyzw);
	output.xyzw = mul(projectionMatrix, output.xyzw);
	//fon't do perspective divide

	return output;
}