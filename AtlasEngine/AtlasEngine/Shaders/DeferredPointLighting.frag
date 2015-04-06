#version 430 core

struct FragmentData_t
{
	vec3 Color;
	vec3 Normal;
	vec3 ViewCoord;
	float AmbientOcclusion;
	uint MaterialID;
};

// sizeof = 40
struct PointLight_t
{
    // std140 alignment      Base Align		Aligned Offset		End
	vec3 Position;         //    16               0              12
	vec3 Color;            //    16               16             28
	float MinDistance;     //     4               28             32            
	float MaxDistance;     //     4               32             36
};

layout(std140, binding = 2) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 Model;				//		16					0			64
	mat4 View;				//		16					64			128
	mat4 Projection;		//		16					128			192
	mat4 InvProjection;     //      16                  192         256
} Transforms;

layout(std140, binding = 10) uniform PointLightBlock
{
    PointLight_t PointLight;
};

void UnpackGBuffer(ivec2 ScreenCoord, out FragmentData_t Fragment);

vec4 ApplyLighting(FragmentData_t Fragment, PointLight_t Light)
{
	vec4 Result = vec4(0.0, 0.0, 0.0, 1.0);

	// Unfilled fragments will have a material id of 0
	if (Fragment.MaterialID != 0)
	{
		// Get light direction and distance
		vec3 L =  (Transforms.View * vec4(Light.Position, 1)).xyz - Fragment.ViewCoord;
		float Distance = length(L);
		L = normalize(L);

		float Attenuation = 1;
		if (Distance < Light.MinDistance)
			Attenuation = 1.0;
		else
		{
			Attenuation = (Light.MaxDistance - Distance) / (Light.MaxDistance - Light.MinDistance);
		}

		// Normal and reflection vectors
		vec3 N = normalize(Fragment.Normal);
		vec3 H = normalize(L - Fragment.ViewCoord);

		// Calc lighting
		float NdotH = max(0.0, dot(N, H));
		float NdotL = max(0.0, dot(N, L));

		vec3 Diffuse = Light.Color * Fragment.Color * NdotL * Attenuation;

		vec3 Specular;
		if(NdotL < 0.0)
			Specular = vec3(0,0,0);
		else
			Specular = Light.Color * pow(NdotH, 12) * Attenuation;
		
		Result += vec4(Diffuse + Specular, 0.0) * Fragment.AmbientOcclusion;
	}
	return Result;
}

void main()
{
	FragmentData_t Fragment;

	UnpackGBuffer(ivec2(gl_FragCoord.xy), Fragment);

	gl_FragColor = ApplyLighting(Fragment, PointLight);
}
