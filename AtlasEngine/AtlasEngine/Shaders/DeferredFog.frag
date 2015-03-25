#version 430 core

layout (binding = 2) uniform sampler2D DepthTexture;

uniform vec3 FogColor = vec3(.5, .5, .5);
uniform float FogDensity = 0.0003;
uniform float MinFog = 0.0;
uniform float MaxFog = 1.0;

layout(std140, binding = 2) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 Model;				//		16					0			64
	mat4 View;				//		16					64			128
	mat4 Projection;		//		16					128			192
	mat4 InvViewProjection;     //      16                  192         256
} Transforms;

void main()
{
	ivec2 ScreenCoord = ivec2(gl_FragCoord.xy);
	vec4 ClipPosition;
	ClipPosition.xy = ScreenCoord * 2.0 - 1.0;
	ClipPosition.z = texture(DepthTexture , ScreenCoord).r * 2.0 - 1.0;
	ClipPosition.w = 1.0;
	
	vec4 ViewPosition = inverse(Transforms.Projection) * ClipPosition;
	float Depth = (ViewPosition.xyz / ViewPosition.w).z;
	
	
	//float Depth = texelFetch(DepthTexture, ScreenCoord, 0).r;

	float FogFactor = clamp(exp(-FogDensity * Depth * Depth), MinFog, MaxFog);

	gl_FragColor = vec4(FogColor, FogFactor);
}


