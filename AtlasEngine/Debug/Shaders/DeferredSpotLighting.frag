#version 430 core

struct FragmentData_t
{
	vec3 Color;
	vec3 Normal;
	vec3 WorldCoord;
	uint MaterialID;
};

// sizeof = 32
struct SpotLight_t
{
    // std140 alignment      Base Align		Aligned Offset		End
	vec3 Position;         //    16               0              12
	vec3 Color;            //    16               16             28
	vec3 Direction;        //    16               32             44
	float MinDistance;     //     4               44             48            
	float MaxDistance;     //     4               48             52
	float SpotExponent;    //     4               52             56
};

layout(std140, binding = 12) uniform SpotLightBlock
{
    SpotLight_t SpotLight;
};

void UnpackGBuffer(ivec2 ScreenCoord, out FragmentData_t Fragment);

vec4 ApplyLighting(FragmentData_t Fragment, SpotLight_t Light)
{
	vec4 Result = vec4(0.0, 0.0, 0.0, 1.0);

	// Unfilled fragments will have a material id of 0
	if (Fragment.MaterialID != 0)
	{
		// Get light direction and distance
		vec3 LightDirection =  Light.Position - Fragment.WorldCoord;
		float Distance = length(LightDirection);
		LightDirection = normalize(LightDirection);

		float Attenuation;
		float SpotFactor = pow(max(0.0, dot(LightDirection, -Light.Direction)), Light.SpotExponent);
		if(Distance > Light.MaxDistance)
			return Result;
		else if (Distance < Light.MinDistance)
			Attenuation = 1.0 * SpotFactor;
		else
		{
			Attenuation = (Light.MaxDistance - Distance) / (Light.MaxDistance - Light.MinDistance) * SpotFactor;
		}

		// Normal and reflection vectors
		vec3 N = normalize(Fragment.Normal);
		vec3 R = reflect(LightDirection, N);

		// Calc lighting
		float NdotR = max(0.0, dot(N, R));
		float NdotL = max(0.0, dot(N, LightDirection));

		vec3 Diffuse = Light.Color * Fragment.Color * NdotL * Attenuation;
		vec3 Specular = Light.Color * pow(NdotR, 12) * Attenuation;
		if(dot(N, LightDirection) < 0.0)
			Specular = vec3(0,0,0);

		Result += vec4(Diffuse + Specular, 0.0);
	}
	return Result;
}

void main()
{
	FragmentData_t Fragment;

	UnpackGBuffer(ivec2(gl_FragCoord.xy), Fragment);

	gl_FragColor = ApplyLighting(Fragment, SpotLight);
}
