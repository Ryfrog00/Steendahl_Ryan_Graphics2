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
	float4 rgba : OCOLOR;
};
OutputVertex main(InputVertex input)
{
	OutputVertex output = (OutputVertex)0;
	output.xyzw = input.xyzw;
	output.rgba = input.rgba;
	// Do math here

	return output;
}