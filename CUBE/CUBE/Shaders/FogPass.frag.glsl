#version 430 core

#include "DeferredCommon.glsl"

layout(std140, binding = 8) uniform FogParamsBlock
{
//   Member				Base Align		Aligned Offset		End
	float Density;   //	    4					0			4
	float Min;       //	    4					4			8
	float Max;       //	    4					8			12
	vec3  Color;   	 //		16					16			32
} FogParams; 

void main()
{
	// linearize depth and get world units of depth
	vec3 Position = GetViewPosition(ivec2(gl_FragCoord.xy));
	float DistanceSquared = Position.x * Position.x + Position.y * Position.y + Position.z * Position.z; // No sqrt since using exp fog
	float FogFactor = clamp(exp(-FogParams.Density * DistanceSquared), FogParams.Min, FogParams.Max);

	gl_FragColor = vec4(FogParams.Color, FogFactor);
}

