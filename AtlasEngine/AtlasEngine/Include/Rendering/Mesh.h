#pragma once

#include <GL\glew.h>
#include <vector>
#include "..\Math\Vector3.h"

struct VoxelVertex
{
	Vector3f Position;
	Vector3f Normal;
	Vector3f Color;
};

/**
* An OpenGL indexed triangle mesh. 
* Triangle data can be added to the mesh before
* sending that data to the GPU. Once all data has
* been feed into this object, it then needs to be activate
* with Activate(), which sends it's information to the GPU,
* before it can be rendered.
*/
class FMesh
{
public:
	/**
	* Defualt Ctor.
	*/
	FMesh();

	/**
	* Deletes buffers held by this object.
	*/
	~FMesh();

	/**
	* Renders vertex data held by this objects' buffers.
	* Can be supplied with different OpenGL render modes.
	*/
	void Render(const GLenum RenderMode = GL_TRIANGLES);

	/**
	* Adds a vertex to the mesh.
	* @param Vertex to add.
	* @return ID of the added triangle. This is used to add
	*	triangles to the mesh with AddTriangle()
	*/
	uint32_t AddVertex(const VoxelVertex& Vertex);

	/**
	* Adds a triangle to the mesh. Vertices must be in CCW order.
	* @param V1 - First vertex ID.
	* @param V2 - Second vertex ID.
	* @param V3 - Third vertex ID.
	*/
	void AddTriangle(const uint32_t V1, const uint32_t V2, const uint32_t V3);

	/**
	* Clears data held internally by this object and empties
	* OpenGL buffer objects held by this object.
	*/
	void Deactivate();

	/**
	* Allocates OpenGL buffer objects and sends vertex data
	* to the GPU.
	*/
	void Activate();

	/**
	* Gets the status of the object.
	*/
	bool IsActive() const;

	/**
	* Get the index count of the mesh.
	*/
	uint32_t GetIndexCount() const;

	/**
	* Get the triangle count of the mesh.
	*/
	uint32_t TriangleCount() const;

	// TODO Implement these functions.
	void MapIndexBuffer(void* Data, uint32_t Size);

	void MapVertexBuffer(void* Data, uint32_t Size);

private:
	enum BufferType
	{
		Vertex,
		Index,
	};

private:
	std::vector<VoxelVertex> mVertices;
	std::vector<uint32_t> mIndices;
	GLuint mVertexArray;
	GLuint mBuffers[2];

	bool mIsActive;
};

inline bool FMesh::IsActive() const
{
	return mIsActive;
}

inline uint32_t FMesh::GetIndexCount() const
{
	return mIndices.size();
}

inline uint32_t FMesh::TriangleCount() const
{
	return mVertices.size() / 3;
}

inline uint32_t FMesh::AddVertex(const VoxelVertex& Vertex)
{
	uint32_t ReturningIndex = mVertices.size();
	mVertices.push_back(Vertex);
	return ReturningIndex;
}

inline void FMesh::AddTriangle(const uint32_t V1, const uint32_t V2, const uint32_t V3)
{
	mIndices.push_back(V1);
	mIndices.push_back(V2);
	mIndices.push_back(V3);
}