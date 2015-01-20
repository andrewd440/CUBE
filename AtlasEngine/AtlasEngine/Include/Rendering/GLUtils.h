#pragma once

#include <GL\glew.h>
#include <GL\GL.h>

#define BUFFER_OFFSET(offset) ((void *)(offset))

namespace GLUtils
{
	/** 
	* Helper function to convert GLSL types to storage sizes 
	* From The Red Book 4.3
	*/
	size_t GetTypeSize(GLenum type);
}