#version 430 core

#include "DeferredCommon.glsl"

layout (binding = 5) uniform sampler1D KernalSamples;
layout (binding = 6) uniform sampler2D NoiseSamples;

uniform int KernalSize = 8;
uniform uint NoiseSize = 4;

out vec4 AO;

void main()
{
	ivec2 ScreenCoord = ivec2(gl_FragCoord.xy);
	ivec2 NoiseCoord = ScreenCoord % int(NoiseSize);
	vec2 TexCoord = vec2(ScreenCoord) / vec2(Resolution);

	vec3 Position = GetViewPosition(ScreenCoord);
	vec3 Normal = GetNormal(ScreenCoord);

	// Construct change of basis about the normal
	vec3 Reflection = vec3(texelFetch(NoiseSamples, NoiseCoord, 0).xy, 0);
	vec3 UAxis = normalize(Reflection - Normal * dot(Reflection, Normal));
	vec3 VAxis = cross(Normal, UAxis);
	mat3 Rotation = mat3(UAxis, VAxis, Normal);

	float Occlusion = 0.0;
	for(int i = 0; i < KernalSize; ++i)
	{
		// Hemisphere sample
		vec3 SamplePosition = Rotation * texelFetch(KernalSamples, i, 0).xyz;
		SamplePosition = Position + SamplePosition;

		vec4 SampleCoord = (Transforms.Projection * vec4(SamplePosition, 1.0));
		SampleCoord.xyz /= SampleCoord.w;
		SampleCoord.xy = SampleCoord.xy * 0.5 + 0.5;

		float SampledDepth = texture(DepthTexture, SampleCoord.xy).r * 2.0 - 1.0;
		Occlusion += (SampledDepth <= SampleCoord.z) ? 1.0 : 0.0;
	}

	Occlusion = 1.0 - Occlusion / KernalSize;
	AO = vec4(vec3(Occlusion), 1.0);
}