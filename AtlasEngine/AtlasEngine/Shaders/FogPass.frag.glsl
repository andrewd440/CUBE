#version 430 core

layout (binding = 2) uniform sampler2D DepthTexture;

layout(std140, binding = 2) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 Model;				//		16					0			64
	mat4 View;				//		16					64			128
	mat4 Projection;		//		16					128			192
	mat4 InvProjection;     //      16                  192         256
} Transforms;


layout(std140, binding = 8) uniform FogParamsBlock
{
//   Member				Base Align		Aligned Offset		End
	float Density;   //	    4					0			4
	float Min;       //	    4					4			8
	float Max;       //	    4					8			12
	vec3  Color;   	 //		16					16			32
} FogParams; 

layout(std140, binding = 1) uniform ProjectionInfoBlock
{
	float Near;
	float Far;
} ProjectionInfo;

float LinearizeDepth(float Depth, float Near, float Far)
{
    float z = Depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * Near) / (Far + Near - z * (Far - Near));	
}

void main()
{
	// linearize depth and get world units of depth
	float Depth = LinearizeDepth(texelFetch(DepthTexture, ivec2(gl_FragCoord.xy), 0).r, ProjectionInfo.Near, ProjectionInfo.Far);
	Depth *= ProjectionInfo.Far;
	float FogFactor = clamp(exp(-FogParams.Density * Depth * Depth), FogParams.Min, FogParams.Max);

	gl_FragColor = vec4(FogParams.Color, FogFactor);
}

