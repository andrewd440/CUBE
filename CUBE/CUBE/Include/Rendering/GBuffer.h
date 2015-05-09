#pragma once
#include <GL\glew.h>
#include <cstdint>
#include "Math\Vector2.h"

template <uint32_t NumMRT>
/**
* Creates Multiple Render Target that can be used a 
* 'G-Buffer' for deferred rendering. This object attaches 2
* texture targets to it framebuffer object. The size of these textures
* and the internal format for both of them is controlled through the
* constructor.
*/
class GBuffer
{
public:
	static const uint32_t MRTCount = NumMRT;

public:
	/**
	* Constructs a framebuffer with 2 texture targets with a specified resolution and internal data format.
	* @param TexResolution - The size, in pixels, of the texture targets.
	* @param DepthAttachment - An allocated depth format texture to be attached to the framebuffer.
	* @param ColorAttachments - Each color attachment to be attached to the framebuffer.
	* @note The lifetime of the texture attachments given to this constructor will be managed by this object.
 	*/
	GBuffer(const Vector2ui& TexResolution, const GLuint DepthAttachment, const GLuint ColorAttachments[MRTCount]);
	~GBuffer();

	GBuffer& operator=(GBuffer&& Other);

	/**
	* Binds the framebuffer held by this object for writing.
	*/
	void Bind();

	/**
	* Sets the draw buffers for the FBO. GBuffer::Bind() must be called prior to this function.
	* @param DrawBuffers - The index of the color attachment to bind for writing.
	* @param BufferCount - The size of DrawBuffers.
	* @note The color buffers and depth buffer are not cleared on this call, you must
	*        clear these buffer with the appropriate GL call.
	*/
	void SetDrawBuffers(const GLenum* DrawBuffers, const uint32_t BufferCount);

	/**
	* Binds the default FBO.
	*/
	void Unbind();

	/**
	* Binds the MRT textures for reading by the shader.
	* The textures will be bound to GL_TEXTURE0 and GL_TEXTURE1
	* The depth texture will be bound to GL_TEXTURE2
	*/
	void StartRead(const uint32_t DepthTexUnit, const uint32_t ColorTexUnits[MRTCount]);

private:
	GLuint mFrameBuffer;
	GLuint mMRTs[NumMRT];
	GLuint mDepth;
	Vector2ui mResolution;
};

#include "GBuffer.inl"
