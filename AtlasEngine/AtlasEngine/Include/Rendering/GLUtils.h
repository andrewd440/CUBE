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

	template <GLenum Target>
	void BufferUnmapper(void*)
	{
		glUnmapBuffer(Target);
		glBindBuffer(Target, 0);
	}

	/**
	* Scope based manager for binding OpenGL buffer
	* objects. This object binds a specified buffer
	* on construct and unbinds that buffer type on destruction.
	*/
	template <GLenum Target>
	struct BufferBinder
	{
		BufferBinder(GLuint BufferID)
		{
			glBindBuffer(Target, BufferID);
		}

		~BufferBinder()
		{
			glBindBuffer(Target, 0);
		}
	};

	/**
	* Scope based manager for binding OpenGL vertex array
	* objects. This object binds a specified vertex array
	* on construct and unbinds that VAO on destruction.
	*/
	struct ArrayBinder
	{
		ArrayBinder(GLuint ArrayID)
		{
			glBindVertexArray(ArrayID);
		}

		~ArrayBinder()
		{
			glBindVertexArray(0);
		}
	};
}

namespace GLAttributePosition
{
	enum
	{
		Position,
		Normal,
		Color,
	};
}