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

struct PointLight_t
{
	vec3 Position;
	uint Pad0;
	vec3 Color;
	uint Pad1; 
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

vec4 ApplyLighting(FragmentData_t Fragment, PointLight_t Light)
{
	const float LightRangeMax = 50;
	const float LightRangeMin = 5;
	int i;
	vec4 result = vec4(0.0, 0.0, 0.0, 1.0);
	if (Fragment.MaterialID != 0)
	{
		vec3 L = Fragment.WorldCoord - Light.Position;
		float dist = length(L);
		L = normalize(L);
		vec3 N = -normalize(Fragment.Normal);
		vec3 R = reflect(-L, N);
		float NdotR = max(0.0, dot(N, R));
		float NdotL = max(0.0, dot(N, L));
		float attenuation = clamp((LightRangeMax - dist) / (LightRangeMax - LightRangeMin), 0, 1);
		vec3 diffuse_color = Light.Color * Fragment.Color * NdotL * attenuation; // attenuation
		vec3 specular_color = Light.Color * pow(NdotR, 12) * attenuation; // attenuation
		result += vec4(diffuse_color + specular_color, 0.0);
	}
	return result;
}

void main()
{
	PointLight_t Light1, Light2;
	Light1.Position = vec3(2100,2080,2000);
	Light2.Position = vec3(2050,2080,2050);

	Light1.Color = vec3(.3,.4,1);
	Light2.Color = vec3(0,1,1);

	FragmentData_t Fragment;

	UnpackGBuffer(ivec2(gl_FragCoord.xy), Fragment);

	gl_FragColor = ApplyLighting(Fragment, Light2) + ApplyLighting(Fragment, Light1);
}
