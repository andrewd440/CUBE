#include "Rendering\DepthRenderTarget.h"


FDepthRenderTarget::FDepthRenderTarget(const Vector2ui TextureResolution)
	: mFrameBuffer(0)
	, mDepthTexture(0)
	, mTextureResolution(TextureResolution)
	, mActiveTexture(GL_TEXTURE0)
{
	glGenFramebuffers(1, &mFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	ASSERT(mFrameBuffer != 0);

	glGenTextures(1, &mDepthTexture);
	SetResolution(TextureResolution);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, mDepthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


FDepthRenderTarget::~FDepthRenderTarget()
{
	glDeleteFramebuffers(1, &mFrameBuffer);
	glDeleteTextures(1, &mDepthTexture);
}

void FDepthRenderTarget::StartWrite()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	glViewport(0, 0, mTextureResolution.x, mTextureResolution.y);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	glDrawBuffer(GL_NONE);
}

void FDepthRenderTarget::EndWrite(const Vector2ui DefaultViewPort)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, DefaultViewPort.x, DefaultViewPort.y);
	glDrawBuffer(GL_BACK);
}

void FDepthRenderTarget::StartRead()
{
	glActiveTexture(GL_TEXTURE0 + mActiveTexture);
	glBindTexture(GL_TEXTURE_2D, mDepthTexture);
}

void FDepthRenderTarget::EndRead()
{
	glActiveTexture(GL_TEXTURE0 + mActiveTexture);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FDepthRenderTarget::SetResolution(const Vector2ui NewResolution)
{
	mTextureResolution = NewResolution;
	glBindTexture(GL_TEXTURE_2D, mDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
		NewResolution.x, NewResolution.y, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set up depth comparison mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// Set up wrapping modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}