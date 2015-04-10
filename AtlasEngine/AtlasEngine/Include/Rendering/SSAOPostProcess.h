#pragma once
#include "IRenderPostProcess.h"
#include "Rendering\ShaderProgram.h"

class FSSAOPostProcess : public IRenderPostProcess
{
public:
	FSSAOPostProcess();
	~FSSAOPostProcess();

	void OnPostLightingPass() override;

private:
	FShaderProgram mSSAOShader;
};

