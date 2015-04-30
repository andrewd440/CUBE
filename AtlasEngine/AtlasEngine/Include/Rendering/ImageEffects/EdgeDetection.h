#pragma once
#include "Rendering\ImageEffects\IImageEffect.h"
#include "Rendering\ShaderProgram.h"

class FEdgeDetection : public IImageEffect
{
public:
	FEdgeDetection();
	~FEdgeDetection();

	void OnPostLightingPass() override;

private:
	FShaderProgram mShader;
};

