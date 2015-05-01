#pragma once

#include <GL\glew.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#define BUFFER_OFFSET(offset) ((void *)(offset))

#ifdef _DEBUG
#define GL_CHECK(Stmt) \
		(Stmt);\
		GLUtils::ErrorCheck(#Stmt, __FILE__, __LINE__);\
		
#else
#define GL_CHECK(Stmt) (Stmt)
#endif

namespace GLUtils
{
	/**
	* Checks for any OpenGL error from the last
	* api call. If an error is produced, a message is printed
	* and the application exited.
	*/
	inline void ErrorCheck(const char* Stmt, const char* Filename, const uint32_t Line)
	{
		GLenum Error = glGetError();
		if (Error != GL_NO_ERROR)
		{
			printf_s("OpenGL error %08x produced\nOperation: %s\nLine: %u\nFile: %s\n", Error, Stmt, Line, Filename);
			abort();
		}
	}

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