#pragma once
#include "IImageEffect.h"
#include "Rendering\ShaderProgram.h"
#include "Rendering\Uniform.h"
#include "Math\Vector2.h"

class FSSAOPostProcess : public IImageEffect
{
public:
	static const uint32_t DEFAULT_KERNAL_SIZE = 8;
	static const uint32_t DEFAULT_NOISE_SIZE = 4;
public:
	FSSAOPostProcess();
	~FSSAOPostProcess();

	void OnPostLightingPass() override;

	void SetRadius(const float Radius);

private:
	void GenerateNoiseTexture(const uint32_t Size);
	void GenerateSampleTexture(const uint32_t KernalSize);

private:
	FShaderProgram mSSAO;
	GLuint         mNoiseTex;
	GLuint         mSampleTex;
};

