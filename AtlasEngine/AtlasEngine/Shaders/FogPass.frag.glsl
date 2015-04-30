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

layout(std140, binding = 4) uniform ResolutionBlock
{
	uvec2 Resolution;
};

vec3 GetViewPosition(sampler2D DepthSampler, ivec2 ScreenCoord, vec2 Resolution);

void main()
{
	// linearize depth and get world units of depth
	vec3 Position = GetViewPosition(DepthTexture, ivec2(gl_FragCoord.xy), vec2(Resolution));
	float DistanceSquared = Position.x * Position.x + Position.y * Position.y + Position.z * Position.z; // No sqrt since using exp fog
	float FogFactor = clamp(exp(-FogParams.Density * DistanceSquared), FogParams.Min, FogParams.Max);

	gl_FragColor = vec4(FogParams.Color, FogFactor);
}

