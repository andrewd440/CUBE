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

// sizeof = 40
struct DirectionalLight_t
{
    // std140 alignment      Base Align		Aligned Offset		End
	vec3 Direction;         //    16               0              12
	vec3 Color;            //    16               16             28
};

layout(std140, binding = 2) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 Model;				//		16					0			64
	mat4 View;				//		16					64			128
	mat4 Projection;		//		16					128			192
	mat4 InvProjection;     //      16                  192         256
} Transforms;

layout(std140, binding = 11) uniform DirectionalLightBlock
{
    DirectionalLight_t DirectionalLight;
};

void UnpackGBuffer(ivec2 ScreenCoord, out FragmentData_t Fragment);
vec4 ApplyLighting(FragmentData_t Fragment, DirectionalLight_t Light);

void main()
{
	FragmentData_t Fragment;

	UnpackGBuffer(ivec2(gl_FragCoord.xy), Fragment);

	gl_FragColor = ApplyLighting(Fragment, DirectionalLight);
}

vec4 ApplyLighting(FragmentData_t Fragment, DirectionalLight_t Light)
{
	vec4 Result = vec4(0.0, 0.0, 0.0, 1.0);

	// Unfilled fragments will have a material id of 0
	if (Fragment.MaterialID != 0)
	{
		// Normal and reflection vectors
		vec3 L = mat3(Transforms.View) * (-Light.Direction);
		vec3 N = normalize(Fragment.Normal);
		vec3 H = normalize(L - Fragment.ViewCoord);

		// Calc lighting
		float NdotH = max(0.0, dot(N, H));
		float NdotL = max(0.0, dot(N, L));

		vec3 Diffuse = Light.Color * Fragment.Color * NdotL;

		vec3 Specular;
		if(NdotL < 0.0)
			Specular = vec3(0,0,0);
		else
			Specular = Light.Color * Fragment.Color * pow(NdotH, 4);

		Result += vec4(Diffuse, 0.0);// * Fragment.AmbientOcclusion;
	}
	return Result;
}