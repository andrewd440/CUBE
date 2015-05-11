#pragma once
#include "IImageEffect.h"
#include "Rendering\ShaderProgram.h"
#include "Rendering\Uniform.h"
#include "Math\Vector2.h"
#include "Math\Vector3.h"

class FSSAOPostProcess : public IImageEffect
{
public:
	static const uint32_t DEFAULT_KERNAL_SIZE = 16;
	static const uint32_t DEFAULT_NOISE_SIZE = 4;
public:
	FSSAOPostProcess();
	~FSSAOPostProcess();

	void OnPostLightingPass() override;

	/**
	* Sets the max radius that contributes
	* to an object being occluded.
	*/
	void SetRadius(const float Radius);

	/**
	* Sets the size of the noise texture to use when
	* taking random samples for each pixel.
	*/
	void SetNoiseSize(const uint32_t Size);

	/**
	* Sets the size of the kernal used to sample for
	* occlusion.
	*/
	void SetKernalSize(const uint32_t Size);

	/**
	* Sets the strength of ambient occlusion values.
	**/
	void SetPower(const float Power);

	/**
	* Sets the global ambient lighting for
	* the scene.
	*/
	void SetGlobalAmbient(const Vector3f& Ambient);

private:
	void GenerateNoiseTexture(const uint32_t Size);
	void GenerateSampleTexture(const uint32_t KernalSize);
	void ResizeRenderTarget(const Vector2ui Size);

private:
	struct
	{
		GLuint FBO;
		GLuint mSSAOTex;
	} mSSAOBuffer;

	FShaderProgram mSSAO;
	GLuint         mNoiseTex;
	GLuint         mSampleTex;

	FShaderProgram mBlur;
};

