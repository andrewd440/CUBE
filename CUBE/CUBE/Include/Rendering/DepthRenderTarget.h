#pragma once

#include <cstdint>
#include <GL\glew.h>

#include "Math\Vector2.h"

/**
* A framebuffer processes and saves depth
* information to a texture of a rendered scene.
*/
class FDepthRenderTarget
{
public:
	/**
	* Default Ctor
	*/
	FDepthRenderTarget(const Vector2ui TextureResolution);
	~FDepthRenderTarget();

	/**
	* Opens the depth texture to be written to
	* by future draw calls.
	*/
	void StartWrite();

	/**
	* Disables the depth texture for writing.
	* @param DefaultViewPort - The gl viewport to return to.
	*/
	void EndWrite(const Vector2ui DefaultViewPort);

	/**
	* Enables the depth texture to be read from
	* in the shader pipeline.
	* @param ActiveTexture - The texture unit to bind to.
	*/
	void StartRead();
	
	/**
	* Disables the depth texture for reading.
	*/
	void EndRead();

	/**
	* Sets the resolution of the texture.
	*/
	void SetResolution(const Vector2ui NewResolution);

	/**
	* Gets the resoluiont of the texture.
	*/
	Vector2ui GetResolution() const { return mTextureResolution; }

	void SetTextureUnit(const GLenum ActiveTexture) { mActiveTexture = ActiveTexture; }

private:
	GLuint      mFrameBuffer;
	GLuint      mDepthTexture;
	GLenum      mActiveTexture;
	Vector2ui   mTextureResolution;
};

