#version 430 core

#include "DeferredCommon.glsl"

uniform uint uBlurSize = 4;

layout (location = 0) out uvec4 Color0;
layout (location = 1) out vec4 Color1;

void main()
{
	ivec2 ScreenCoord = ivec2(gl_FragCoord.xy);
	float Sum = 0.0;
	for(uint y = 0; y < uBlurSize; ++y)
	{
		for(uint x = 0; x < uBlurSize; ++x)
		{
			Sum += GetAO(ScreenCoord + ivec2(x, y));
		}
	}

	Sum /= (uBlurSize * uBlurSize);
	Color0 = texelFetch(GBuffer0, ScreenCoord, 0);
	Color1.r = Sum;
}

