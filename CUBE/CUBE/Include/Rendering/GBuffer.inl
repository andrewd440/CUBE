#include "Rendering\GLUtils.h"
#include "Rendering\GLBindings.h"

//template <uint32_t NumMRT>
//GBuffer<NumMRT>::GBuffer(const Vector2ui& TexResolution, const GLuint DepthAttachment, const GLuint ColorAttachments[MRTCount])
//	: mFrameBuffer(0)
//	, mDepth(0)
//	, mResolution(TexResolution)
//{
//	// Create targets with input parameters
//	//glGenTextures(BufferType::Count, mTextureIDs);
//	//ASSERT(mTextureIDs[0] != 0 && mTextureIDs[1] != 0);
//
//	//glBindTexture(GL_TEXTURE_2D, mTextureIDs[BufferType::Color0]);
//	//glTexStorage2D(GL_TEXTURE_2D, 1, InternalFormat1, TexResolution.x, TexResolution.y);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	//glBindTexture(GL_TEXTURE_2D, mTextureIDs[BufferType::Color1]);
//	//glTexStorage2D(GL_TEXTURE_2D, 1, InternalFormat2, TexResolution.x, TexResolution.y);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	//// Depth texture
//	//glBindTexture(GL_TEXTURE_2D, mTextureIDs[BufferType::Depth]);
//	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, TexResolution.x, TexResolution.y);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	//float DepthBorder[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	//GL_CHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, DepthBorder));
//
//	glGenFramebuffers(1, &mFrameBuffer);
//	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
//	ASSERT(mFrameBuffer != 0);
//
//	// Set attachments
//	for (uint32_t i = 0; i < MRTCount; ++i)
//	{
//		GL_CHECK(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, mMRTs[i], 0));
//	}
//
//	GL_CHECK(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepth, 0));
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}
//
//template <uint32_t NumMRT>
//GBuffer<NumMRT>& GBuffer<NumMRT>::operator=(GBuffer<NumMRT>&& Other)
//{
//	mFrameBuffer = Other.mFrameBuffer;
//	for (uint32_t i = 0; i < MRTCount; ++i)
//	{
//		mMRTs[i] = Other.mMRTs[i];
//	}
//	mDepth = Other.mDepth;
//	mResolution = Other.mResolution;
//
//	Other.mFrameBuffer = 0;
//	for (uint32_t i = 0; i < MRTCount; ++i)
//	{
//		Other.mMRTs[i] = 0;
//	}
//	Other.mDepth = 0;
//	Other.mResolution = Vector2ui{ 0, 0 };
//
//	return *this;
//}
//
//template <uint32_t NumMRT>
//GBuffer<NumMRT>::~GBuffer()
//{
//	glDeleteFramebuffers(1, &mFrameBuffer);
//	glDeleteTextures(MRTCount, mMRTs);
//	glDeleteTextures(1, mDepth);
//}
//
//template <uint32_t NumMRT>
//void GBuffer<NumMRT>::Bind()
//{
//	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
//	glViewport(0, 0, mResolution.x, mResolution.y);
//}
//
//template <uint32_t NumMRT>
//void GBuffer<NumMRT>::SetDrawBuffers(const GLenum* DrawBuffers, const uint32_t BufferCount)
//{
//	GL_CHECK(glDrawBuffers(AttachmentCount, DrawBuffers));
//}
//
//template <uint32_t NumMRT>
//void GBuffer<NumMRT>::Unbind()
//{ 
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glDrawBuffer(GL_BACK);
//}
//
//template <uint32_t NumMRT>
//void GBuffer<NumMRT>::StartRead(const uint32_t DepthTexUnit, const uint32_t ColorTexUnits[MRTCount])
//{
//	glActiveTexture(GL_TEXTURE0 + DepthTexUnit);
//	glBindTexture(GL_TEXTURE_2D, mDepth);
//
//	for (uint32_t i = 0; i < MRTCount; ++i)
//	{
//		glActiveTexture(GL_TEXTURE0 + ColorTexUnits[i]);
//		glBindTexture(GL_TEXTURE_2D, mMRTs[i]);
//	}
//}