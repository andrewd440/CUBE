#version 430 core

layout (binding = 0) uniform usampler2D GBuffer0;
layout (binding = 2) uniform sampler2D DepthTexture;

struct FragmentSSAOData_t
{
	vec3 Position;
	vec3 Normal;
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

const int SampleCount = 8;
const vec2 Poisson16[] = vec2[](    // These are the Poisson Disk Samples
                                vec2( -0.94201624,  -0.39906216 ),
                                vec2(  0.94558609,  -0.76890725 ),
								vec2( -0.91588581,   0.45771432 ),
                                vec2(  0.34495938,   0.29387760 ),
								vec2( -0.094184101, -0.92938870 ),
                                vec2( -0.81544232,  -0.87912464 ),
                                vec2( -0.38277543,   0.27676845 ),
                                vec2(  0.97484398,   0.75648379 ),
                                vec2(  0.44323325,  -0.97511554 ),
                                vec2(  0.53742981,  -0.47373420 ),
                                vec2( -0.26496911,  -0.41893023 ),
                                vec2(  0.79197514,   0.19090188 ),
                                vec2( -0.24188840,   0.99706507 ),
                                vec2( -0.81409955,   0.91437590 ),
                                vec2(  0.19984126,   0.78641367 ),
                                vec2(  0.14383161,  -0.14100790 )
                               );

uniform vec2 SampleRadius;// = vec2(20.0 / 1920.0, 20.0 / 1080.0);
uniform float MaxDistance;// = 4.0;

vec3 GetViewPosition(sampler2D DepthSampler, ivec2 ScreenCoord, vec2 Resolution);

void UnpackSSAOFragment(ivec2 ScreenCoord, out FragmentSSAOData_t Fragment)
{
	Fragment.Position = GetViewPosition(DepthTexture, ScreenCoord, vec2(Resolution));

	uvec4 Data0 = texelFetch(GBuffer0, ScreenCoord, 0);
	vec2 ColorZNormX = unpackHalf2x16(Data0.y);
	Fragment.Normal = normalize(vec3(ColorZNormX.y, unpackHalf2x16(Data0.z)));
}

void main()
{
	FragmentSSAOData_t FragmentData;
	ivec2 FragPosition = ivec2(gl_FragCoord.xy);
	UnpackSSAOFragment(FragPosition, FragmentData);

	vec2 FResolution = vec2(Resolution);
	vec2 TexCoord = vec2(FragPosition) / FResolution;

	float Occlusion = 1.0;
	vec2 SampleRadiusFrag = (SampleRadius / FragmentData.Position.z);

	for(uint i = 0; i < SampleCount; i++)
	{
		vec2 SampleTexCoord = TexCoord + (Poisson16[i] * SampleRadiusFrag);
		vec3 SamplePosition = GetViewPosition(DepthTexture, ivec2(SampleTexCoord * FResolution), FResolution);
		vec3 SampleDirection = normalize(SamplePosition - FragmentData.Position);

		float NdotS = max(dot(FragmentData.Normal, SampleDirection), 0.0);
		float Distance = distance(SamplePosition, FragmentData.Position);

		float a = 1.0 - smoothstep(MaxDistance, MaxDistance * 2, Distance);
		
		Occlusion += a * NdotS;
	}

	Occlusion = (Occlusion / SampleCount);
	gl_FragColor = vec4(Occlusion, Occlusion, Occlusion, Occlusion);
}