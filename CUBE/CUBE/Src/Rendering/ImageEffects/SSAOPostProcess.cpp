#include "Rendering\ImageEffects\SSAOPostProcess.h"
#include "ResourceHolder.h"

FSSAOPostProcess::FSSAOPostProcess()
	: IImageEffect()
	, mSSAOShader()
	, mSampleRadius()
	, mMaxDistance()
{

	FShader SSAOFrag{ L"Shaders/SSAOPass.frag.glsl", GL_FRAGMENT_SHADER };
	mSSAOShader.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mSSAOShader.AttachShader(SShaderHolder::Get("DeferredLightingCommon.frag"));
	mSSAOShader.AttachShader(SSAOFrag);
	mSSAOShader.LinkProgram();
	mSSAOShader.Use();

	mSampleRadius.Bind(mSSAOShader.GetID(), "SampleRadius");
	mMaxDistance.Bind(mSSAOShader.GetID(), "MaxDistance");
}


FSSAOPostProcess::~FSSAOPostProcess()
{

}

void FSSAOPostProcess::OnPostLightingPass()
{
	mSSAOShader.Use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ONE);
	glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void FSSAOPostProcess::SetSampleRadius(const Vector2f& Radius)
{
	mSSAOShader.Use();
	mSampleRadius.SetVector(1, &Radius);
}

void FSSAOPostProcess::SetMaxDistance(const float Distance)
{
	mSSAOShader.Use();
	mMaxDistance.SetUniform(Distance);
}