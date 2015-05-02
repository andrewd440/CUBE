#include "Rendering\ImageEffects\FogPostProcess.h"
#include "Rendering\GLBindings.h"
#include "ResourceHolder.h"
namespace
{
	struct FogBlockOffsets
	{
		enum : uint32_t
		{
			Density = 0,
			Min = 4,
			Max = 8,
			Color = 16,
			Size = 32
		};
	};
}

FFogPostProcess::FFogPostProcess()
	: mShaderProgram()
	, mFogParamBlock(GLUniformBindings::FogParamBlock, FogBlockOffsets::Size)
{
	FShader FragShader{ L"Shaders/FogPass.frag.glsl", GL_FRAGMENT_SHADER };
	mShaderProgram.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mShaderProgram.AttachShader(FragShader);
	mShaderProgram.LinkProgram();
}


FFogPostProcess::~FFogPostProcess()
{
}


void FFogPostProcess::OnPostLightingPass()
{
	mShaderProgram.Use();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisable(GL_BLEND);
}

void FFogPostProcess::SetDensity(const float Density)
{
	mFogParamBlock.SetData(FogBlockOffsets::Density, (uint8_t*)&Density, sizeof(float));
}

void FFogPostProcess::SetBounds(const float Min, const float Max)
{
	mFogParamBlock.SetData(FogBlockOffsets::Min, (uint8_t*)&Min, sizeof(float));
	mFogParamBlock.SetData(FogBlockOffsets::Max, (uint8_t*)&Max, sizeof(float));
}

void FFogPostProcess::SetColor(const Vector3f& Color)
{
	mFogParamBlock.SetData(FogBlockOffsets::Color, Color);
}