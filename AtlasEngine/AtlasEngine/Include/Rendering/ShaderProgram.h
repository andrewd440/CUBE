#pragma once

#include <GL\glew.h>
#include <GL\GL.h>
#include <vector>

/**
* Class for creating an OpenGL shader object.
* Shader objects encapsulated by this class can
* be attached to shader programs from FShaderProgram.
* Shader objects create with this class will be deleted
* when the objects destructor is called.
*/
class FShader
{
public:
	/**
	* Construct an OpenGL shader object.
	* @param SourceFile - The source of the shader.
	* @param ShaderType - The type of the shader.
	*/
	FShader(const wchar_t* SourceFile, GLenum ShaderType);

	/**
	* Dtor
	* Delete the shader object from OpenGL.
	*/
	~FShader();

	// Disable copying of shaders
	FShader(const FShader&) = delete;
	FShader& operator=(const FShader&) = delete;

	/**
	* Retrieve the OpenGL shader ID.
	*/
	GLuint GetID() const;

	/**
	* Retrieve the OpenGL shader type.
	*/
	GLenum GetType() const;

private:
	/**
	* Reads a shader source file from a filepath.
	*/
	std::string ReadShader(const wchar_t* SourceFile) const;

#ifndef NDEBUG
	/**
	* Checks for errors in a shader. If errors are
	* found, log info is printed to the debug log.
	*/
	void CheckShaderErrors(GLuint Shader) const;
#endif

private:
	GLuint mID;
	GLenum mType;
};


/**
* Class for creating an OpenGL shader program.
* Shader programs encapsulated by this class will be deleted
* when this object goes out of scope, so there is no need to 
* explicitly call glDeleteProgram with FShaderProgram::GetID().
*/
class FShaderProgram
{
public:
	FShaderProgram();

	/**
	* Default ctor.
	* Creates a shader program.
	* @param Shaders - A list of compiled shaders to attach to
	*					this program.
	*/
	explicit FShaderProgram(const std::initializer_list<const FShader*> Shaders);

	/**
	* Dtor
	* Delete the shader program from OpenGL.
	*/
	~FShaderProgram();

	/**
	* Attaches a shader type to this shader program from a source file. 
	* Once this function completes, the program will only be compiled and
	* attached to the program, so linking the program will still be required.
	* @param Shader - The shader object to attach.
	*/
	void AttachShader(const FShader& Shader);

	/**
	* Links this shader program with previously attached shaders.
	* Once the linking is complete, all previously attacheds shaders
	* are detached from this program.
	*/
	void LinkProgram();

	/**
	* Returns the GLuint id of the program.
	**/
	GLuint GetID() const { return mID; }

	/**
	* Tells OpenGL to use this
	* shader program.
	*/
	void Use() { glUseProgram(mID); }

private:

#ifndef NDEBUG
	/**
	* Checks for errors in this program. If errors are
	* found, log info is printed to the debug log.
	*/
	void CheckProgramErrors();
#endif

private:
	GLuint mID; // ID for the GL program.
};

