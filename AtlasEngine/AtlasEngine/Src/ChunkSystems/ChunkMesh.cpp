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
	mRenderData[0] = VertexRenderDataPtr{ new VertexRenderData{} };
	mRenderData[1] = VertexRenderDataPtr{ new VertexRenderData{} };

	mVertices[0] = VertexPositionDataPtr{ new VertexPositionData{} };
	mVertices[1] = VertexPositionDataPtr{ new VertexPositionData{} };

	mIndices[0] = IndexDataPtr{ new IndexData{} };
	mIndices[1] = IndexDataPtr{ new IndexData{} };

	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(2, mBuffers);

	glBindVertexArray(mVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, mBuffers[Buffer::Vertex]);
			glVertexAttribIPointer(GLAttributePosition::Position, 1, GL_UNSIGNED_INT, 0, BUFFER_OFFSET(0));
			glEnableVertexAttribArray(GLAttributePosition::Position);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[Buffer::Index]);
	glBindVertexArray(0);

}


FChunkMesh::~FChunkMesh()
{
	glDeleteBuffers(2, mBuffers);
	glDeleteVertexArrays(1, &mVertexArray);
}

void FChunkMesh::AddVertexPositions(VertexPositionDataPtr Vertices)
{
	mVertices[!mActiveBuffer] = std::move(Vertices);
}

void FChunkMesh::AddRenderData(VertexRenderDataPtr RenderVertices)
{
	mRenderData[!mActiveBuffer] = std::move(RenderVertices);
}

void FChunkMesh::AddIndices(IndexDataPtr Indices)
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(RenderData) * mRenderData[!mActiveBuffer]->size(), mRenderData[!mActiveBuffer]->data(), BufferUsageMode);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[Buffer::Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mIndices[!mActiveBuffer]->size(), mIndices[!mActiveBuffer]->data(), BufferUsageMode);

	mActiveBuffer = !mActiveBuffer;
}

void FChunkMesh::ClearBackBuffer()
{
	mRenderData[!mActiveBuffer] = VertexRenderDataPtr{ new VertexRenderData{} };
	mVertices[!mActiveBuffer]   = VertexPositionDataPtr{ new VertexPositionData{} };
	mIndices[!mActiveBuffer]    = IndexDataPtr{ new IndexData{} };
}