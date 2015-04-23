#pragma once

#include <vector>
#include <memory>
#include <atomic>

#include "GL\glew.h"
#include "Math\Vector3.h"
#include "Common.h"


/**
* A double buffered mesh used to construct and render
* chunks.
*/
class FChunkMesh
{
public:
	// Empty structs for choosing which buffer to get data from.
	struct BackBuffer{};
	struct FrontBuffer{};

	/**
	* Compressed rendering data for a chunk vertex.
	*/
	struct RenderData
	{
		uint32_t BlockType : 8;
		uint32_t pad : 3;
		uint32_t NormalIndex : 3;
		uint32_t z : 6;
		uint32_t y : 6;
		uint32_t x : 6;
	};

public:
	static GLuint BufferUsageMode;

	using VertexPositionData = std::vector<Vector3f>;
	using VertexPositionDataPtr = std::unique_ptr<VertexPositionData>;

	using VertexRenderData = std::vector<RenderData>;
	using VertexRenderDataPtr = std::unique_ptr<VertexRenderData>;

	using IndexData = std::vector<uint32_t>;
	using IndexDataPtr = std::unique_ptr<IndexData>;

public:
	FChunkMesh();
	~FChunkMesh();

	/**
	* Add vertex data to the active buffer. This is uncompressed vertex
	* position data to be used this physics.
	*/
	void AddVertexPositions(VertexPositionDataPtr Vertices);

	/**
	* Add compressed vertex data to the active buffer that will be used
	* for rendering.
	*/
	void AddRenderData(VertexRenderDataPtr RenderVertices);

	/**
	* Add index data to the active buffer.
	*/
	void AddIndices(IndexDataPtr Indices);

	/**
	* Render this mesh using the active buffer.
	*/
	void Render(GLenum RenderMode = GL_TRIANGLES);

	/**
	* Swap the active buffer with the back buffer.
	*/
	void SwapBuffer();

	/**
	* Clear data held by the inactive vertex and index
	* buffers.
	*/
	void ClearBackBuffer();

	/**
	* Get vertex position data for the inactive mesh buffer.
	*/
	const Vector3f* GetVertexData(BackBuffer) const;

	/**
	* Get vertex position data for the active mesh buffer.
	*/
	const Vector3f* GetVertexData(FrontBuffer) const;

	/**
	* Get the vertex count for the inactive mesh buffer.
	*/
	uint32_t GetVertexCount(BackBuffer) const;

	/**
	* Get the vertex count for the active mesh buffer.
	*/
	uint32_t GetVertexCount(FrontBuffer) const;

	/**
	* Get index data for the inactive mesh buffer.
	*/
	const uint32_t* GetIndexData(BackBuffer) const;

	/**
	* Get index data for the active mesh buffer.
	*/
	const uint32_t* GetIndexData(FrontBuffer) const;

	/**
	* Get the index count for the inavtive mesh buffer.
	*/
	uint32_t GetIndexCount(BackBuffer) const;

	/**
	* Get the index count for the active mesh buffer.
	*/
	uint32_t GetIndexCount(FrontBuffer) const;

private:
	struct Buffer
	{
		enum
		{
			Vertex,
			Index
		};
	};

private:
	VertexRenderDataPtr   mRenderData[2];
	VertexPositionDataPtr mVertices[2];
	IndexDataPtr          mIndices[2];

	// GL buffers held by this object
	GLuint mVertexArray;
	GLuint mBuffers[2];

	std::atomic_bool mActiveBuffer;
};

inline const Vector3f* FChunkMesh::GetVertexData(FChunkMesh::BackBuffer) const
{
	return mVertices[!mActiveBuffer]->data();
}

inline uint32_t FChunkMesh::GetVertexCount(FChunkMesh::BackBuffer) const
{
	return mVertices[!mActiveBuffer]->size();
}

inline const uint32_t* FChunkMesh::GetIndexData(FChunkMesh::BackBuffer) const
{
	return mIndices[!mActiveBuffer]->data();
}

inline uint32_t FChunkMesh::GetIndexCount(FChunkMesh::BackBuffer) const
{
	return mIndices[!mActiveBuffer]->size();
}

inline const Vector3f* FChunkMesh::GetVertexData(FChunkMesh::FrontBuffer) const
{
	return mVertices[mActiveBuffer]->data();
}

inline uint32_t FChunkMesh::GetVertexCount(FChunkMesh::FrontBuffer) const
{
	return mVertices[mActiveBuffer]->size();
}

inline const uint32_t* FChunkMesh::GetIndexData(FChunkMesh::FrontBuffer) const
{
	return mIndices[mActiveBuffer]->data();
}

inline uint32_t FChunkMesh::GetIndexCount(FChunkMesh::FrontBuffer) const
{
	return mIndices[mActiveBuffer]->size();
}