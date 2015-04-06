#version 430 core

layout (binding = 0) uniform usampler2D GBuffer0;
layout (binding = 1) uniform sampler2D GBuffer1;
layout (binding = 2) uniform sampler2D DepthTexture;

struct FragmentData_t
{
	vec3 Color;
	vec3 Normal;
	vec3 ViewCoord;
	float AmbientOcclusion;
	uint MaterialID;
};

layout(std140, binding = 2) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 Model;				//		16					0			64
	mat4 View;				//		16					64			128
	mat4 Projection;		//		16					128			192
	mat4 InvProjection;		//      16                  192         256
} Transforms;

vec3 GetViewPosition(ivec2 ScreenCoord)
{
	vec2 NDC = ((ScreenCoord * 2.0) / vec2(1920, 1080)) - 1.0;
	float Depth = texelFetch(DepthTexture, ScreenCoord, 0).r * 2.0 - 1.0;
	vec4 View = Transforms.InvProjection * vec4(NDC, Depth, 1.0);
	return View.xyz / View.w;
}

void UnpackGBuffer(ivec2 ScreenCoord, out FragmentData_t Fragment)
{
	uvec4 Data0 = texelFetch(GBuffer0, ivec2(ScreenCoord), 0);
	vec4 Data1 = texelFetch(GBuffer1, ivec2(ScreenCoord), 0);

	vec2 ColorZNormX = unpackHalf2x16(Data0.y);
	Fragment.Color = vec3(unpackHalf2x16(Data0.x), ColorZNormX.x);
	Fragment.Normal = normalize(vec3(ColorZNormX.y, unpackHalf2x16(Data0.z)));
	Fragment.MaterialID = Data0.w;

	Fragment.ViewCoord = GetViewPosition(ScreenCoord);
	Fragment.AmbientOcclusion = float(Data1.x);
}