#version 430 core

layout (binding = 2) uniform sampler2D DepthTexture;

uniform vec3 FogColor = vec3(.5, .5, .5);
uniform float FogDensity = 0.00004;
uniform float MinFog = 0.0;
uniform float MaxFog = 1.0;

layout(std140, binding = 2) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 Model;				//		16					0			64
	mat4 View;				//		16					64			128
	mat4 Projection;		//		16					128			192
	mat4 InvProjection;     //      16                  192         256
} Transforms;

float LinearizeDepth(float depth)
{
    float near = 0.1; 
    float far = 330.0; 
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near) / (far + near - z * (far - near));	
}

void main()
{	
	float Depth = LinearizeDepth(texelFetch(DepthTexture, ivec2(gl_FragCoord.xy), 0).r) * 330.0;

	float FogFactor = clamp(exp(-FogDensity * Depth * Depth), MinFog, MaxFog);

	gl_FragColor = vec4(FogColor, FogFactor);
}


