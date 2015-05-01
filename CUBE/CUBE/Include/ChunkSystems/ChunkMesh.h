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
	struct Vertex
	{
		Vector3f Position;
		uint8_t  BlockType;
		uint8_t  NormalID;
	};

public:
	static GLuint BufferUsageMode;

	using VertexData = std::vector<Vertex>;
	using VertexDataPtr = std::unique_ptr<VertexData>;

	using IndexData = std::vector<uint32_t>;
	using IndexDataPtr = std::unique_ptr<IndexData>;

public:
	FChunkMesh();
	~FChunkMesh();

	/**
	* Add vertex data to the active buffer. This is uncompressed vertex
	* position data to be used this physics.
	*/
	void AddVertexData(VertexDataPtr Vertices);

	/**
	* Add index data to the active buffer.
	*/
	void AddIndexData(IndexDataPtr Indices);

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
	const Vertex* GetVertexData(BackBuffer) const;

	/**
	* Get vertex position data for the active mesh buffer.
	*/
	const Vertex* GetVertexData(FrontBuffer) const;

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
	VertexDataPtr   mVertices[2];
	IndexDataPtr    mIndices[2];

	// GL buffers held by this object
	GLuint mVertexArray;
	GLuint mBuffers[2];

	std::atomic_bool mActiveBuffer;
};

inline const FChunkMesh::Vertex* FChunkMesh::GetVertexData(FChunkMesh::BackBuffer) const
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

inline const FChunkMesh::Vertex* FChunkMesh::GetVertexData(FChunkMesh::FrontBuffer) const
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