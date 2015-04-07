#pragma once
#include "IRenderPostProcess.h"
#include "Rendering\ShaderProgram.h"
#include "Rendering\UniformBlockStandard.h"

/**
* Applies a depth based fog post-process by sampling
* the depth texture.
*/
class FFogPostProcess : public IRenderPostProcess
{
public:
	FFogPostProcess();
	~FFogPostProcess();

	void OnPostLightingPass() override;

	void SetColor(const Vector3f& Color);
	void SetDensity(const float Density);
	void SetBounds(const float Min, const float Max);

private:
	FShaderProgram      mShaderProgram;
	EZGL::FUniformBlock mFogParamBlock;
};

