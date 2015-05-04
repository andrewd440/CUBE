layout (binding = 0) uniform usampler2D GBuffer0;
layout (binding = 2) uniform sampler2D DepthTexture;

struct FragmentData_t
{
	vec3 Color;
	vec3 Normal;
	vec3 ViewCoord;
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

layout(std140, binding = 4) uniform ResolutionBlock
{
	uvec2 Resolution;
};

layout(std140, binding = 1) uniform ProjectionInfoBlock
{
	float Near;
	float Far;
} ProjectionInfo;

vec3 GetViewPosition(sampler2D DepthSampler, ivec2 ScreenCoord, vec2 Resolution)
{
	vec2 NDC = ((ScreenCoord * 2.0) / Resolution) - 1.0;
	float Depth = texelFetch(DepthSampler, ScreenCoord, 0).r * 2.0 - 1.0;
	vec4 View = Transforms.InvProjection * vec4(NDC, Depth, 1.0);
	return View.xyz / View.w;
}

float GetLinearDepth(ivec2 ScreenCoord)
{
	float Depth = texelFetch(DepthTexture, ScreenCoord, 0).r;
    float z = Depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * ProjectionInfo.Near) / (ProjectionInfo.Far + ProjectionInfo.Near - z * (ProjectionInfo.Far - ProjectionInfo.Near));	
}

void UnpackGBuffer(ivec2 ScreenCoord, out FragmentData_t Fragment)
{
	uvec4 Data0 = texelFetch(GBuffer0, ivec2(ScreenCoord), 0);

	vec2 ColorZNormX = unpackHalf2x16(Data0.y);
	Fragment.Color = vec3(unpackHalf2x16(Data0.x), ColorZNormX.x);
	Fragment.Normal = normalize(vec3(ColorZNormX.y, unpackHalf2x16(Data0.z)));
	Fragment.MaterialID = Data0.w;

	Fragment.ViewCoord = GetViewPosition(DepthTexture, ScreenCoord, vec2(Resolution));
}