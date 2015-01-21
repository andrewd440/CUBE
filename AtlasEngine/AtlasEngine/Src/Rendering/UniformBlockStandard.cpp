#include "..\..\Include\Rendering\UniformBlockStandard.h"

void FUniformBlockStandard::GetBlockSize(const char* BlockName, GLint& SizeOut)
{
	// Get the current shader program and block index to use for retrieving block info
	GLint Program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &Program);

	GLuint BlockIndex = glGetUniformBlockIndex(Program, BlockName);

	glGetActiveUniformBlockiv(Program, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &SizeOut);
}

FUniformBlockStandard::FUniformBlockStandard(const char* BlockName, GLuint BindingIndexToSet, const uint32_t BlockSize)
	: mData(BlockSize)
	, mBufferID()
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
	: mData(BlockSize)
	, mBufferID()
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

void FUniformBlockStandard::SendBuffer()
{
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferID);
	glBufferData(GL_UNIFORM_BUFFER, mData.size(), mData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void FUniformBlockStandard::SetData(const uint32_t DataOffset, const uint8_t* Data, const uint32_t DataSize)
{
	std::memcpy(mData.data() + DataOffset, Data, DataSize);
}