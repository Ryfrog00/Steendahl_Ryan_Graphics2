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
	float4 t : TIMER;
	float4 c : CAMERA;
};

cbuffer SHADER_VARS : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 time;
	float4 camPos;
};


OutputVertex main(InputVertex input, uint instanceId : SV_instanceID)
{
	input.xyz *= 20.0f;
	float3 offset = { 20.0f, 1.0f, 50.0f };
	input.xyz += offset * instanceId;

	OutputVertex output = (OutputVertex)0;
	output.c = camPos;
	output.xyzw = float4(input.xyz, 1);
	output.t = time;
	//make the texture wave
	//output.xyzw.y += 1.5f * sin(output.xyzw.x + 40.0f * time.w);
	//output.xyzw.y += 3.0f * sin(output.xyzw.z + 20.0f * time.w);
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