#version 430 core

#include "DeferredLightingCommon.frag"

// sizeof = 40
struct DirectionalLight_t
{
    // std140 alignment      Base Align		Aligned Offset		End
	vec3 Direction;         //    16               0              12
	vec3 Color;            //    16               16             28
};

layout(std140, binding = 11) uniform DirectionalLightBlock
{
    DirectionalLight_t DirectionalLight;
};

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

		//vec3 Specular;
		//if(NdotL < 0.0)
		//	Specular = vec3(0,0,0);
		//else
		//	Specular = Light.Color * Fragment.Color * pow(NdotH, 4);

		Result += vec4(Diffuse, 0.0);
	}
	return Result;
}