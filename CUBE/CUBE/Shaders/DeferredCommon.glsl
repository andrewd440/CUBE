#include "UniformBlocks.glsl"

layout (binding = 0) uniform usampler2D GBuffer0;
layout (binding = 2) uniform sampler2D DepthTexture;

struct FragmentData_t
{
	vec3 Color;
	vec3 Normal;
	vec3 ViewCoord;
	uint MaterialID;
};

vec3 GetViewPosition(ivec2 ScreenCoord)
{
	vec2 NDC = ((ScreenCoord * 2.0) / Resolution) - 1.0;
	float Depth = texelFetch(DepthTexture, ScreenCoord, 0).r * 2.0 - 1.0;
	vec4 View = Transforms.InvProjection * vec4(NDC, Depth, 1.0);
	return View.xyz / View.w;
}

vec3 GetNormal(ivec2 ScreenCoord)
{
	uvec4 Data0 = texelFetch(GBuffer0, ScreenCoord, 0);
	vec2 ColorZNormX = unpackHalf2x16(Data0.y);
	return normalize(vec3(ColorZNormX.y, unpackHalf2x16(Data0.z)));
}

vec3 GetColor(ivec2 ScreenCoord)
{
	uvec4 Data0 = texelFetch(GBuffer0, ScreenCoord, 0);
	vec2 ColorZNormX = unpackHalf2x16(Data0.y);
	return vec3(unpackHalf2x16(Data0.x), ColorZNormX.x);
}

float GetLinearDepth(ivec2 ScreenCoord)
{
	float Depth = texelFetch(DepthTexture, ScreenCoord, 0).r;
    float z = Depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * ProjectionInfo.Near * ProjectionInfo.Far) / (ProjectionInfo.Far + ProjectionInfo.Near - z * (ProjectionInfo.Far - ProjectionInfo.Near));	
}

void UnpackGBuffer(ivec2 ScreenCoord, out FragmentData_t Fragment)
{
	uvec4 Data0 = texelFetch(GBuffer0, ScreenCoord, 0);

	vec2 ColorZNormX = unpackHalf2x16(Data0.y);
	Fragment.Color = vec3(unpackHalf2x16(Data0.x), ColorZNormX.x);
	Fragment.Normal = normalize(vec3(ColorZNormX.y, unpackHalf2x16(Data0.z)));
	Fragment.MaterialID = Data0.w;

	Fragment.ViewCoord = GetViewPosition(ScreenCoord);
}