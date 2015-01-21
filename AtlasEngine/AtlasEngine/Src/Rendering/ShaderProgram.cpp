#include "..\..\Include\Rendering\ShaderProgram.h"
#include "..\..\Include\SystemFile.h"
#include "..\..\Include\Debugging\ConsoleOutput.h"

#include <GL\glew.h>
#include <GL\GL.h>
#include "SFML\Window\Context.hpp"
#include <cstdint>
#include <vector>

/////////////////////////
//// FShader ////////////

FShader::FShader(const wchar_t* SourceFile, GLenum ShaderType)
	: mID()
	, mType(ShaderType)
{
	mID = glCreateShader(ShaderType);

	const std::string ShaderSource = ReadShader(SourceFile);
	const char* SourcePtr = ShaderSource.c_str();
	glShaderSource(mID, 1, &SourcePtr, nullptr);

	glCompileShader(mID);

#ifndef NDEBUG
	CheckShaderErrors(mID);
#endif
}

FShader::~FShader()
{
	glDeleteShader(mID);
}

GLuint FShader::GetID() const
{
	return mID;
}

GLenum FShader::GetType() const
{
	return mType;
}

std::string FShader::ReadShader(const wchar_t* SourceFile) const
{
	IFileSystem& FileSystem = IFileSystem::GetInstance();
	auto ShaderFile = FileSystem.OpenReadable(SourceFile);

	if (ShaderFile)
	{
		const uint32_t ShaderSize = ShaderFile->GetFileSize();

		std::string ShaderSource;
		ShaderSource.resize(ShaderSize);
		ShaderFile->Read((uint8_t*)ShaderSource.data(), ShaderSize);
		//ShaderSource.append('\0'); // add the null terminator that is missing.
		return ShaderSource;
	}

	return std::string();
}


#ifndef NDEBUG
void FShader::CheckShaderErrors(GLuint Shader) const
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
#endif

/////////////////////////
//// FShaderProgram /////

FShaderProgram::FShaderProgram(const std::initializer_list<const FShader*> Shaders)
	: mID(glCreateProgram())
{
	for (auto Itr = Shaders.begin(); Itr != Shaders.end(); Itr++)
	{
		AttachShader(*(*Itr));
	}

	LinkProgram();

#ifndef NDEBUG
	CheckProgramErrors();
#endif
}

FShaderProgram::~FShaderProgram()
{
	glDeleteProgram(mID);
}

void FShaderProgram::AttachShader(const FShader& Shader)
{
	glAttachShader(mID, Shader.GetID());
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

#ifndef NDEBUG
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