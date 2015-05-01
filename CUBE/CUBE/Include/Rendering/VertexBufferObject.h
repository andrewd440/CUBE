#pragma once

#include <GL\glew.h>
#include <vector>
#include <cstdint>
#include <memory>
#include "GLUtils.h"

/**
* Encapsulates an OpenGL vertex buffer object. Any operations on this object
* should be preceded with SetActive(true) and followed by SetActive(false). These
* objects can be grouped with FVertexArrayObject to encapsulation with OpenGLs VAO.
*/
class FVertexBufferObject
{
public:
	/**
	* Creates an OpenGL vertex buffer object.
	*/
	FVertexBufferObject();

	/**
	* Creates an OpenGL VBO with a preallocated size and usage type.
	* @param BufferSize - Size of the buffer to preallocate.
	* @param Usage - The usage model for this buffer.
	*/
	FVertexBufferObject(const GLsizeiptr BufferSize, const GLenum Usage);

	// Allow temp copies.
	FVertexBufferObject(FVertexBufferObject&& Other);

	// Disable vertex buffer copying
	FVertexBufferObject(const FVertexBufferObject&) = delete;
	FVertexBufferObject& operator=(const FVertexBufferObject&) = delete;

	/**
	* Deletes the vertex buffer object held by this object.
	*/
	~FVertexBufferObject();

	/**
	* Allocates and sends data to this buffer. Equivalent to glBufferData.
	* @param BufferSize - The total size of the buffer
	* @param Data - The data to fill the buffer with.
	* @param Usage - The usage model for this buffer.
	*/
	void SetData(const GLsizeiptr BufferSize, const GLvoid* Data, GLenum Usage);

	/**
	* Fills a previously allocated buffer object with data. Equivalent to glBufferSubData.
	* @param Offset - Position in the buffer to begin filling data.
	* @param DataSize - The size of the data to be filled.
	* @param Data - The data to put in the buffer.
	*/
	void SetSubData(const GLintptr Offset, const GLsizeiptr DataSize, const GLvoid* Data);

	/**
	* Retrieve a pointer to the buffer held by this object.
	* @param Access - Type of access required for the memory.
	* @return The pointer is returned in a unique_ptr object with a custom deleter
	* that unbinds and unmappeds this buffer.
	*/
	std::unique_ptr<void, void(*)(void*)> MapBuffer(const GLenum Access);

	/**
	* Sets an attribute within this buffer. Equivalent to glVertexAttribPointer.
	* After an attribute is set, it will still need to be activated with EnableAttribute.
	* @param Index - The index of the attribute in the shader.
	* @param ElementsPerVert - The number of elements in that are sent to each vertex.
	* @param ElementType - The data type of the data elements.
	* @param Normalize - If the data should be normalized.
	* @param Stride - The difference, in bytes, between one vertex data and the next.
	* @param Offset - Where the first element is in the buffer.
	*/
	void SetAttribute(const GLuint Index, GLint ElementsPerVert, const GLenum ElementType, bool Normalize, const GLsizei Stride, const GLuint Offset);

	/**
	* Enables a previously set attribute.
	*/
	void EnableAttribute(const GLuint Index);

	/**
	* Disables a previously set attribute.
	*/
	void DisableAttribute(const GLuint Index);

	/**
	* Binds this buffer to the OpenGL context.
	*/
	void SetActive(bool IsActive);

private:
	GLuint mBufferID;
};

/**
* Encapsulates an OpenGL vertex array object. Any operations on this object
* should be preceded with SetActive(true) and followed by SetActive(false).
* FVertexBufferObjects can be used from within this class to encapsulate
* the action performed on those buffers.
*/
class FVertexArrayObject
{
public:
	/**
	* Creates an OpenGL VAO
	*/
	FVertexArrayObject();

	/**
	* Creates an OpenGL VAO and a number of
	* FVertexBufferObjects from within this object.
	*/
	FVertexArrayObject(const GLuint NumBuffers);

	// Allow temp copies
	FVertexArrayObject(FVertexArrayObject&& Other);

	// Disable copying
	FVertexArrayObject(const FVertexArrayObject&) = delete;
	FVertexArrayObject& operator=(const FVertexArrayObject&) = delete;

	/**
	* Deletes the OpenGL VAO held within this object.
	*/
	~FVertexArrayObject();

	/**
	* Adds a VBO to this object.
	*/
	void AddBuffer();

	/**
	* Adds a VBO with a preallocated size and usage type to this object.
	*/
	void AddBuffer(const GLsizeiptr BufferSize, const GLenum Usage);

	/**
	* Retrieves a VBO previously added to this object.
	*/
	FVertexBufferObject& GetBuffer(GLuint Index);

	/**
	* Activates/Deactivates this VAO.
	*/
	void SetActive(bool IsActive);

private:
	GLuint mVertexArrayID;
	std::vector<FVertexBufferObject> mBufferObjects;
};



////////////////////////////////////////////////////////////////////////
/////// Inline FVertexBufferObject /////////////////////////////////////
////////////////////////////////////////////////////////////////////////

inline FVertexBufferObject::FVertexBufferObject()
	: mBufferID()
{
	glGenBuffers(1, &mBufferID);
}

inline FVertexBufferObject::FVertexBufferObject(const GLsizeiptr BufferSize, const GLenum Usage)
	: mBufferID()
{
	glGenBuffers(1, &mBufferID);
	SetActive(true);
	SetData(BufferSize, nullptr, Usage);
	SetActive(false);
}

inline FVertexBufferObject::~FVertexBufferObject()
{
	glDeleteBuffers(1, &mBufferID);
}

inline FVertexBufferObject::FVertexBufferObject(FVertexBufferObject&& Other)
{
	mBufferID = std::move(Other.mBufferID);
	Other.mBufferID = 0;
}

inline void FVertexBufferObject::SetData(const GLsizeiptr BufferSize, const GLvoid* Data, GLenum Usage)
{
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Data, Usage);
}

inline void FVertexBufferObject::SetSubData(const GLintptr Offset, const GLsizeiptr DataSize, const GLvoid* Data)
{
	glBufferSubData(GL_ARRAY_BUFFER, Offset, DataSize, Data);
}

inline std::unique_ptr<void, void(*)(void*)> FVertexBufferObject::MapBuffer(const GLenum Access)
{
	return std::unique_ptr<void, void(*)(void*)>(glMapBuffer(GL_ARRAY_BUFFER, Access), GLUtils::BufferUnmapper<GL_ARRAY_BUFFER>);
}

inline void FVertexBufferObject::SetAttribute(const GLuint Index, GLint ElementsPerVert, const GLenum ElementType, bool Normalize, const GLsizei Stride, const GLuint Offset)
{
	glVertexAttribPointer(Index, ElementsPerVert, ElementType, Normalize, Stride, BUFFER_OFFSET(Offset));
}

inline void FVertexBufferObject::EnableAttribute(const GLuint Index)
{
	glEnableVertexAttribArray(Index);
}

inline void FVertexBufferObject::DisableAttribute(const GLuint Index)
{
	glDisableVertexAttribArray(Index);
}

inline void FVertexBufferObject::SetActive(bool IsActive)
{
	IsActive ? glBindBuffer(GL_ARRAY_BUFFER, mBufferID) : glBindBuffer(GL_ARRAY_BUFFER, 0);
}

////////////////////////////////////////////////////////////////////////
/////// Inline FVertexArrayObject //////////////////////////////////////
////////////////////////////////////////////////////////////////////////

inline FVertexArrayObject::FVertexArrayObject()
	: mVertexArrayID(0)
	, mBufferObjects()
{
	glGenVertexArrays(1, &mVertexArrayID);
}

inline FVertexArrayObject::FVertexArrayObject(const GLuint NumBuffers)
	: mVertexArrayID()
	, mBufferObjects(NumBuffers)
{
	glGenVertexArrays(1, &mVertexArrayID);
}

inline FVertexArrayObject::FVertexArrayObject(FVertexArrayObject&& Other)
{
	mVertexArrayID = std::move(Other.mVertexArrayID);
	Other.mVertexArrayID = 0;
}

inline FVertexArrayObject::~FVertexArrayObject()
{
	glDeleteVertexArrays(1, &mVertexArrayID);
}

inline void FVertexArrayObject::AddBuffer()
{
	mBufferObjects.push_back(FVertexBufferObject());
}

inline void FVertexArrayObject::AddBuffer(const GLsizeiptr BufferSize, const GLenum Usage)
{
	mBufferObjects.push_back(FVertexBufferObject(BufferSize, Usage));
}

inline FVertexBufferObject& FVertexArrayObject::GetBuffer(GLuint Index)
{
	return mBufferObjects[Index];
}

inline void FVertexArrayObject::SetActive(bool IsActive)
{
	IsActive ? glBindVertexArray(mVertexArrayID) : glBindVertexArray(0);
}