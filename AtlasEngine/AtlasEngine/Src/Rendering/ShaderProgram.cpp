#include "..\..\Include\Rendering\ShaderProgram.h"
#include "..\..\Include\SystemFile.h"
#include "..\..\Include\Debugging\ConsoleOutput.h"

#include <GL\glew.h>
#include <GL\GL.h>
#include "SFML\Window\Context.hpp"
#include <cstdint>
#include <vector>

FShaderProgram::FShaderProgram(const wchar_t* VertexSrc, const wchar_t* FragmentSrc, const wchar_t* GeometrySrc)
	: mID(glCreateProgram())
{
	if (VertexSrc) AddShader(GL_VERTEX_SHADER, VertexSrc);
	if (FragmentSrc) AddShader(GL_FRAGMENT_SHADER, FragmentSrc);
	if (GeometrySrc) AddShader(GL_GEOMETRY_SHADER, GeometrySrc);
	LinkProgram();

#ifndef NDEBUG
	CheckProgramErrors();
#endif
}

FShaderProgram::~FShaderProgram()
{
	glDeleteProgram(mID);
}

void FShaderProgram::AddShader(GLenum ShaderType, const wchar_t* FilePath)
{
	const char* ShaderSource = ReadShader(FilePath);

	// Create shader and point it to the source data
	GLuint Shader = glCreateShader(ShaderType);
	glShaderSource(Shader, 1, &ShaderSource, nullptr);
	delete[] ShaderSource;

	glCompileShader(Shader);

#ifndef NDEBUG
	CheckShaderErrors(Shader);
#endif

	glAttachShader(mID, Shader);
	glDeleteShader(Shader);
}

void FShaderProgram::LinkProgram() const
{
	glLinkProgram(mID);

	// Detach all attached shaders
	GLsizei ShaderCount;
	glGetProgramiv(mID, GL_ATTACHED_SHADERS, &ShaderCount);

	std::vector<GLuint> AttachedShaders(ShaderCount);
	glGetAttachedShaders(mID, ShaderCount, nullptr, &AttachedShaders[0]);

	for (GLsizei i = 0; i < ShaderCount; i++)
	{
		glDetachShader(mID, AttachedShaders[i]);
	}
}

char* FShaderProgram::ReadShader(const wchar_t* FilePath) const
{
	IFileSystem& FileSystem = IFileSystem::GetInstance();
	auto ShaderFile = FileSystem.OpenReadable(FilePath);

	if (ShaderFile)
	{
		const uint32_t ShaderSize = ShaderFile->GetFileSize();

		char* ShaderSource = new char[ShaderSize + 1];
		ShaderFile->Read((uint8_t*)ShaderSource, ShaderSize);
		ShaderSource[ShaderSize] = '\0'; // add the null terminator that is missing.
		return ShaderSource;
	}

	return new char[1];
}

#ifndef NDEBUG
void FShaderProgram::CheckShaderErrors(GLuint Shader)
{
	// Retrieve the status of the shader
	GLint Success = 0;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);

	if (Success == GL_TRUE)
		return;
	
	// If here, we have an error, so retrieves it's info
	GLint LogSize = 0;
	glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &LogSize);

	// Get the info log and pring to debug output
	std::vector<char> LogInfo(LogSize);
	glGetShaderInfoLog(Shader, LogSize, nullptr, &LogInfo[0]);
	FDebug::PrintF(&LogInfo[0]);
}

void FShaderProgram::CheckProgramErrors()
{
	// Retrieve the status of the program
	GLint IsLinked = 0;
	glGetProgramiv(mID, GL_LINK_STATUS, &IsLinked);

	if (IsLinked == GLU_TRUE)
		return;

	// If here, we have an error so retrieve and print it's info
	GLint LogSize = 0;
	glGetProgramiv(mID, GL_INFO_LOG_LENGTH, &LogSize);

	std::vector<char> LogInfo(LogSize);
	glGetProgramInfoLog(mID, LogSize, nullptr, &LogInfo[0]);
	FDebug::PrintF(&LogInfo[0]);
}
#endif