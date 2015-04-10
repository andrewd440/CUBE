#include "..\..\Include\Rendering\SSAOPostProcess.h"


FSSAOPostProcess::FSSAOPostProcess()
	: IRenderPostProcess()
	, mSSAOShader()
{
	FShader FullScreenQuadShader{ L"Shaders/FullScreenQuad.vert", GL_VERTEX_SHADER };
	FShader SSAOFrag{ L"Shaders/SSAOPass.frag.glsl", GL_FRAGMENT_SHADER };
	mSSAOShader.AttachShader(FullScreenQuadShader);
	mSSAOShader.AttachShader(SSAOFrag);
	mSSAOShader.LinkProgram();
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