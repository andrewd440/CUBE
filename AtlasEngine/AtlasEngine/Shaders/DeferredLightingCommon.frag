#version 430 core

layout (binding = 0) uniform usampler2D GBuffer0;
layout (binding = 1) uniform sampler2D GBuffer1;

struct FragmentData_t
{
	vec3 Color;
	vec3 Normal;
	vec3 WorldCoord;
	uint MaterialID;
};

void UnpackGBuffer(ivec2 ScreenCoord, out FragmentData_t Fragment)
{
	uvec4 Data0 = texelFetch(GBuffer0, ivec2(ScreenCoord), 0);
	vec4 Data1 = texelFetch(GBuffer1, ivec2(ScreenCoord), 0);

	vec2 ColorZNormX = unpackHalf2x16(Data0.y);
	Fragment.Color = vec3(unpackHalf2x16(Data0.x), ColorZNormX.x);
	Fragment.Normal = normalize(vec3(ColorZNormX.y, unpackHalf2x16(Data0.z)));
	Fragment.MaterialID = Data0.w;

	Fragment.WorldCoord = vec3(Data1.xyz);
}
