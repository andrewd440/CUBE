#version 430 core

#include "DeferredCommon.glsl"

layout (binding = 7) uniform sampler2D AOTex;

uniform uint uBlurSize = 4;
uniform vec3 uAmbient = vec3(.3, .3, .3);

out vec4 oColor;

void main()
{
	ivec2 ScreenCoord = ivec2(gl_FragCoord.xy);
	float Sum = 0.0;
	for(uint y = 0; y < uBlurSize; ++y)
	{
		for(uint x = 0; x < uBlurSize; ++x)
		{
			Sum += texelFetch(AOTex, ScreenCoord + ivec2(x, y), 0).r;
		}
	}

	Sum /= (uBlurSize * uBlurSize);
	oColor = vec4(Sum * uAmbient * GetColor(ScreenCoord), 1);
}

