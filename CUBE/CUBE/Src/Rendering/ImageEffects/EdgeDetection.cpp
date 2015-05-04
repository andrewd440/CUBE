#include "Rendering\ImageEffects\EdgeDetection.h"
#include "ResourceHolder.h"

FEdgeDetection::FEdgeDetection()
	: mShader()
{
	FShader Frag{ L"Shaders/EdgeDetection.frag.glsl", GL_FRAGMENT_SHADER };
	mShader.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mShader.AttachShader(Frag);
	mShader.LinkProgram();
}


FEdgeDetection::~FEdgeDetection()
{
}

void FEdgeDetection::OnPostLightingPass()
{
	mShader.Use();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_MIN);
	//glDisable(GL_BLEND);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}