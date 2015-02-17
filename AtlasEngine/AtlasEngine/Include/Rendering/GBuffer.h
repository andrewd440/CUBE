#pragma once
#include <GL\glew.h>
#include <cstdint>
#include "Math\Vector2.h"

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
	/**
	* Constructs a framebuffer with 2 texture targets with a specified resolution and internal data format.
	* @param TexResolution - The size, in pixels, of the texture targets.
	* @param InternalFormat1 - The format of the first texture target.
	* @param InternalFormat2 - The format of the second texture target.
 	*/
	GBuffer(const Vector2ui& TexResolution, const GLenum InternalFormat1, const GLenum InternalFormat2);
	~GBuffer();

	/**
	* Binds the framebuffer and texture targets held by this object for writing.
	* The draw buffers are GL_COLOR_ATTACHMENT0 and GL_COLOR_ATTACHMENT1
	*/
	void StartWrite();

	/**
	* Unbinds this framebuffer from writes.
	*/
	void EndWrite();

	/**
	* Binds the MRT textures for reading by the shader.
	* The textures will be bound to GL_TEXTURE0 and GL_TEXTURE1
	*/
	void StartRead();

	/**
	* Unbinds the MRT textures from reading by the shader.
	*/
	void EndRead();

private:
	GLuint mFrameBuffer;
	GLuint mTextureIDs[3];
	Vector2ui mResolution;
};

