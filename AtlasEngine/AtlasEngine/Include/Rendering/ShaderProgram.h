#pragma once

#include <GL\glew.h>
#include <GL\GL.h>

/**
* Class for creating an OpenGL shader program.
* Shader programs encapsulated by this class will be deleted
* when this object goes out of scope, so there is no need to 
* explicitly call glDeleteProgram with FShaderProgram::GetID().
*/
class FShaderProgram
{
public:
	/**
	* Default ctor.
	* Creates a shader program with possible inputs
	* for a vertex, fragment, and geometry shader.
	* @param VertexSrc - Filename of the vertex shader source.
	* @param FragmentSrc - Filename of the fragment shader source.
	* @param GeometrySrc - Filename of the geometry shader source.
	*/
	FShaderProgram(const wchar_t* VertexSrc = nullptr,
					const wchar_t* FragmentSrc = nullptr,
					const wchar_t* GeometrySrc = nullptr);
	~FShaderProgram();

	/**
	* Attaches a shader type to this shader program from a source file. 
	* Once this function completes, the program will only be compiled and
	* attached to the program, so linking the program will still be required.
	* @param ShaderType - Type of shader to attach.
	* @param FilePath - Location of the shader source file.
	*/
	void AddShader(GLenum ShaderType, const wchar_t* FilePath);

	/**
	* Links this shader program with previously attached shaders.
	* Once the linking is complete, all previously attacheds shaders
	* are detached from this program.
	*/
	void LinkProgram() const;

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
	/**
	* Reads a shader source file from a filepath.
	*/
	char* ReadShader(const wchar_t* FilePath) const; 

#ifndef NDEBUG
	/**
	* Checks for errors in a shader. If errors are
	* found, log info is printed to the debug log.
	*/
	void CheckShaderErrors(GLuint Shader);

	/**
	* Checks for errors in this program. If errors are
	* found, log info is printed to the debug log.
	*/
	void CheckProgramErrors();
#endif

private:
	GLuint mID; // ID for the GL program.
};

