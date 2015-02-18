#include "..\..\Include\Rendering\UniformBlockStandard.h"
#include "..\..\Include\Rendering\GLUtils.h"

void FUniformBlockStandard::GetBlockSize(const char* BlockName, GLint& SizeOut)
{
	// Get the current shader program and block index to use for retrieving block info
	GLint Program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &Program);

	GLuint BlockIndex = glGetUniformBlockIndex(Program, BlockName);

	glGetActiveUniformBlockiv(Program, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &SizeOut);
}

FUniformBlockStandard::FUniformBlockStandard(const char* BlockName, GLuint BindingIndexToSet, const uint32_t BlockSize)
	: mBufferID()
{
	GLint Program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &Program);

	GLuint BlockIndex = glGetUniformBlockIndex(Program, BlockName);
	glUniformBlockBinding(Program, BlockIndex, BindingIndexToSet);

	glGenBuffers(1, &mBufferID);
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferID);
	glBufferData(GL_UNIFORM_BUFFER, BlockSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, BindingIndexToSet, mBufferID);
}

FUniformBlockStandard::FUniformBlockStandard(const uint32_t BindingIndex, const uint32_t BlockSize)
	: mBufferID()
{
	glGenBuffers(1, &mBufferID);
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferID);
	glBufferData(GL_UNIFORM_BUFFER, BlockSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, BindingIndex, mBufferID);
}

FUniformBlockStandard::~FUniformBlockStandard()
{
	glDeleteBuffers(1, &mBufferID);
}

void FUniformBlockStandard::SetData(const uint32_t DataOffset, const uint8_t* Data, const uint32_t DataSize)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferID);
	glBufferSubData(GL_UNIFORM_BUFFER, DataOffset, DataSize, Data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void* FUniformBlockStandard::MapBuffer(GLenum Access) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferID);
	return glMapBuffer(GL_UNIFORM_BUFFER, Access);
}
