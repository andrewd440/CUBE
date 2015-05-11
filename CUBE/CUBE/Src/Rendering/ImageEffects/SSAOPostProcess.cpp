#include "Rendering\ImageEffects\SSAOPostProcess.h"
#include "ResourceHolder.h"
#include "Math\Vector3.h"
#include "Math\FMath.h"
#include "Rendering\GLUtils.h"
#include "Rendering\GLBindings.h"
#include "Rendering\RenderSystem.h"
#include "Rendering\Screen.h"
#include <random>

FSSAOPostProcess::FSSAOPostProcess()
	: IImageEffect()
	, mSSAO()
	, mNoiseTex(0)
	, mSampleTex(0)
{
	mSSAOBuffer.FBO = 0;
	mSSAOBuffer.mSSAOTex = 0;

	FRenderSystem::OnResolutionChange.AddListener<FSSAOPostProcess, &FSSAOPostProcess::ResizeRenderTarget>(this);

	FShader SSAOFrag{ L"Shaders/SSAOPass.frag.glsl", GL_FRAGMENT_SHADER };
	mSSAO.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mSSAO.AttachShader(SSAOFrag);
	mSSAO.LinkProgram();
	
	FShader BlurFrag{ L"Shaders/BlurPass.frag.glsl", GL_FRAGMENT_SHADER };
	mBlur.AttachShader(SShaderHolder::Get("FullScreenQuad.vert"));
	mBlur.AttachShader(BlurFrag);
	mBlur.LinkProgram();

	GenerateNoiseTexture(DEFAULT_NOISE_SIZE);
	GenerateSampleTexture(DEFAULT_KERNAL_SIZE);
	ResizeRenderTarget(SScreen::GetResolution());
}

FSSAOPostProcess::~FSSAOPostProcess()
{
	glDeleteTextures(1, &mNoiseTex);
	glDeleteTextures(1, &mSampleTex);
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
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mSSAOBuffer.FBO));

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	mSSAO.Use();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);
	mBlur.Use();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void FSSAOPostProcess::SetRadius(const float Radius)
{
	mSSAO.SetUniform("uRadius", Radius);
}

void FSSAOPostProcess::SetNoiseSize(const uint32_t Size)
{
	GenerateNoiseTexture(Size);
	mSSAO.SetUniform("uNoiseSize", Size);
	mBlur.SetUniform("uNoiseSize", Size);
}

void FSSAOPostProcess::SetKernalSize(const uint32_t Size)
{
	GenerateSampleTexture(Size);
	mSSAO.SetUniform("uKernalSize", Size);
}

void FSSAOPostProcess::SetPower(const float Power)
{
	mSSAO.SetUniform("uPower", Power);
}

void FSSAOPostProcess::SetGlobalAmbient(const Vector3f& Ambient)
{
	mBlur.SetVector("uAmbient", 1, &Ambient);
}

void FSSAOPostProcess::ResizeRenderTarget(const Vector2ui Size)
{
	if (mSSAOBuffer.FBO != 0)
	{
		// Using buffer immutable textures, so just reallocate
		glDeleteFramebuffers(1, &mSSAOBuffer.FBO);
		glDeleteTextures(1, &mSSAOBuffer.mSSAOTex);
	}

	GL_CHECK(glGenFramebuffers(1, &mSSAOBuffer.FBO));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mSSAOBuffer.FBO));
	GL_CHECK(glViewport(0, 0, Size.x, Size.y));
		GL_CHECK(glGenTextures(1, &mSSAOBuffer.mSSAOTex));
		GL_CHECK(glActiveTexture(GL_TEXTURE0 + GLTextureBindings::SSAOTexture));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mSSAOBuffer.mSSAOTex));
			GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16F, Size.x, Size.y));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CHECK(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mSSAOBuffer.mSSAOTex, 0));
		GL_CHECK(glDrawBuffer(GL_COLOR_ATTACHMENT0));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}