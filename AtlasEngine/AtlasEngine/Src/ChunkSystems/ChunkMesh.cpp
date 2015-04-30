#include "ChunkSystems\ChunkMesh.h"
#include "Rendering\GLBindings.h"
#include "Rendering\GLUtils.h"

GLuint FChunkMesh::BufferUsageMode = GL_STATIC_DRAW;

FChunkMesh::FChunkMesh()
	: mVertexArray(0)
	, mActiveBuffer()
{
	mActiveBuffer = false;

	// Setup vertex and index data with dummy object
	// to prevent nullptr references
	mVertices[0] = VertexDataPtr{ new VertexData{} };
	mVertices[1] = VertexDataPtr{ new VertexData{} };

	mIndices[0] = IndexDataPtr{ new IndexData{} };
	mIndices[1] = IndexDataPtr{ new IndexData{} };

	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(2, mBuffers);

	glBindVertexArray(mVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, mBuffers[Buffer::Vertex]);
			glVertexAttribPointer(GLAttributePosition::Position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
			glEnableVertexAttribArray(GLAttributePosition::Position);
			glVertexAttribIPointer(GLAttributePosition::ChunkData, 1, GL_UNSIGNED_INT, sizeof(Vertex), BUFFER_OFFSET(offsetof(struct Vertex, BlockType)));
			glEnableVertexAttribArray(GLAttributePosition::ChunkData);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[Buffer::Index]);
	glBindVertexArray(0);

}


FChunkMesh::~FChunkMesh()
{
	glDeleteBuffers(2, mBuffers);
	glDeleteVertexArrays(1, &mVertexArray);
}

void FChunkMesh::AddVertexData(VertexDataPtr VertexData)
{
	mVertices[!mActiveBuffer] = std::move(VertexData);
}

void FChunkMesh::AddIndexData(IndexDataPtr Indices)
{
	mIndices[!mActiveBuffer] = std::move(Indices);
}

void FChunkMesh::Render(GLenum RenderMode)
{
	glBindVertexArray(mVertexArray);
	glDrawElements(RenderMode, mIndices[mActiveBuffer]->size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
}

void FChunkMesh::SwapBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, mBuffers[Buffer::Vertex]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices[!mActiveBuffer]->size(), mVertices[!mActiveBuffer]->data(), BufferUsageMode);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[Buffer::Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mIndices[!mActiveBuffer]->size(), mIndices[!mActiveBuffer]->data(), BufferUsageMode);

	mActiveBuffer = !mActiveBuffer;
}

void FChunkMesh::ClearBackBuffer()
{
	mVertices[!mActiveBuffer]   = VertexDataPtr{ new VertexData{} };
	mIndices[!mActiveBuffer]    = IndexDataPtr{ new IndexData{} };
}