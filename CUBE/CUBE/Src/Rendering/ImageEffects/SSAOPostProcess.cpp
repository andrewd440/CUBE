#include "Rendering\ImageEffects\SSAOPostProcess.h"
#include "ResourceHolder.h"
#include "Math\Vector3.h"
#include "Math\FMath.h"
#include "Rendering\GLUtils.h"
#include "Rendering\GLBindings.h"
#include <random>

FSSAOPostProcess::FSSAOPostProcess()
	: IImageEffect()
	, mSSAO()
	, mNoiseTex(0)
	, mSampleTex(0)
{

	FShader SSAOFrag{ L"Shaders/SSAOPass.frag.glsl", GL_FRAGMENT_SHADER };
	mSSAO.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mSSAO.AttachShader(SSAOFrag);
	mSSAO.LinkProgram();
	
	GenerateNoiseTexture(DEFAULT_NOISE_SIZE);
	GenerateSampleTexture(DEFAULT_KERNAL_SIZE);
}


FSSAOPostProcess::~FSSAOPostProcess()
{

}

void FSSAOPostProcess::GenerateNoiseTexture(const uint32_t Size)
{
	std::default_random_engine Generator;
	std::uniform_real_distribution<float> UniDistribution{ -1.0f, 1.0f };

	std::unique_ptr<Vector2f[]> NoiseValues{ new Vector2f[Size * Size] };
	for (uint32_t i = 0; i < Size * Size; i++)
	{
		NoiseValues[i].x = UniDistribution(Generator);
		NoiseValues[i].y = UniDistribution(Generator);
		NoiseValues[i].Normalize();
	}

	if (mNoiseTex != 0)
		GL_CHECK(glDeleteTextures(1, &mNoiseTex));

	GL_CHECK(glGenTextures(1, &mNoiseTex));
	GL_CHECK(glActiveTexture(GL_TEXTURE0 + GLTextureBindings::SSAONoise));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mNoiseTex));
		GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, Size, Size));
		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Size, Size, GL_RG, GL_FLOAT, NoiseValues.get()));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GL_CHECK(glActiveTexture(GL_TEXTURE0));
}

void FSSAOPostProcess::GenerateSampleTexture(const uint32_t KernalSize)
{
	std::default_random_engine Generator;
	std::uniform_real_distribution<float> UniDistribution{ -1.0f, 1.0f };

	// Generate random values within a hemisphere
	std::unique_ptr<Vector3f[]> SampleValues{ new Vector3f[KernalSize] };
	for (uint32_t i = 0; i < KernalSize; i++)
	{
		SampleValues[i].x = UniDistribution(Generator);
		SampleValues[i].y = UniDistribution(Generator);
		SampleValues[i].z = (UniDistribution(Generator) + 1) / 2.0f;
		SampleValues[i].Normalize();

		// Scale so more samples are closer to origin
		float Scale = (float)i / (float)KernalSize;
		SampleValues[i] *= FMath::Lerp(0.1f, 1.0f, Scale * Scale);
	}

	if (mSampleTex != 0)
		GL_CHECK(glDeleteTextures(1, &mSampleTex));

	GL_CHECK(glGenTextures(1, &mSampleTex));
	GL_CHECK(glActiveTexture(GL_TEXTURE0 + GLTextureBindings::SSAOSamples));
	GL_CHECK(glBindTexture(GL_TEXTURE_1D, mSampleTex));
		GL_CHECK(glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGB32F, KernalSize));
		GL_CHECK(glTexSubImage1D(GL_TEXTURE_1D, 0, 0, KernalSize, GL_RGB, GL_FLOAT, SampleValues.get()));
		GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

	GL_CHECK(glActiveTexture(GL_TEXTURE0));
}

void FSSAOPostProcess::OnPostLightingPass()
{
	mSSAO.Use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ONE);
	glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
	glDisable(GL_BLEND);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void FSSAOPostProcess::SetRadius(const float Radius)
{

}