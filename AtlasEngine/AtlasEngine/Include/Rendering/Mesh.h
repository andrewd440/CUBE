#pragma once
#include <GL\glew.h>
#include <vector>
#include "..\Math\Vector3.h"

/**
* Voxel vertex (P, N, C)
*/
struct FVoxelVertex
{
	Vector3f Position;
	Vector3f Normal;
	Vector3f Color;
};

template <typename Vertex>
/**
* An OpenGL indexed triangle mesh. 
* Triangle data can be added to the mesh before
* sending that data to the GPU. Once all data has
* been feed into this object, it then needs to be activate
* with Activate(), which sends it's information to the GPU,
* before it can be rendered.
*/
class IMesh
{
public:
	using VertexType = Vertex;
public:
	/**
	* Defualt Ctor.
	*/
	FMesh();

	/**
	* Copy Ctor.
	* Copies vertex and index data from Other
	* and opens new OpenGL buffers for its data.
	*/
	FMesh(const FMesh& Other);

	/**
	* Move Ctor.
	* Steals vertex data, index data and OpenGL buffers
	* from Other.
	*/
	FMesh(FMesh&& Other);

	/**
	* Deletes buffers held by this object.
	*/
	~FMesh();


	/**
	* Copies vertex and index data from Other
	*/
	FMesh& operator=(const FMesh& Other);

	/**
	* Deletes OpenGL buffers held by this object and
	* vertex data, index data and OpenGL buffers
	* from Other.
	*/
	FMesh& operator=(FMesh&& Other);

	/**
	* Renders vertex data held by this objects' buffers.
	* Can be supplied with different OpenGL render modes.
	*/
	void Render(const GLenum RenderMode = GL_TRIANGLES);

	/**
	* Adds a vertex to the mesh.
	* @param Vertex to add.
	* @return ID of the added vertex. This is used to add
	*	triangles to the mesh with AddTriangle()
	*/
	uint32_t AddVertex(const VertexType& Vertex);

	/**
	* Adds a list of vertices to the mesh.
	* @param Vertex - The vertex list.
	* @param Count - The number of vertices in this list.
	* @return The ID of the first vertex added.
	*/
	uint32_t AddVertex(const VertexType* Vertex, const uint32_t Count);

	/**
	* Adds a triangle to the mesh. Vertices must be in CCW order.
	* @param V1 - First vertex ID.
	* @param V2 - Second vertex ID.
	* @param V3 - Third vertex ID.
	*/
	void AddTriangle(const uint32_t V1, const uint32_t V2, const uint32_t V3);

	/**
	* Adds a list of triangles to the mesh. Vertices must be in CCW order.
	* @param Indices - The list of vertex indices for each triangle.
	* @param Count - Number of triangles to add.
	*/
	void AddTriangle(const uint32_t* Indices, const uint32_t Count);

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
	std::vector<VertexType> mVertices;
	std::vector<uint32_t> mIndices;
	GLuint mVertexArray;
	GLuint mBuffers[2];

	bool mIsActive;
};

#include "Mesh.inl"

template <typename T>
inline bool FMesh<T>::IsActive() const
{
	return mIsActive;
}

template <typename T>
inline uint32_t FMesh<T>::GetIndexCount() const
{
	return mIndices.size();
}

template <typename T>
inline uint32_t FMesh<T>::TriangleCount() const
{
	return mVertices.size() / 3;
}

template <typename T>
inline uint32_t FMesh<T>::AddVertex(const typename FMesh<T>::VertexType& Vertex)
{
	uint32_t ReturningIndex = mVertices.size();
	mVertices.push_back(Vertex);
	return ReturningIndex;
}

template <typename T>
inline uint32_t FMesh<T>::AddVertex(const typename FMesh<T>::VertexType* Vertex, const uint32_t Count)
{
	uint32_t ReturningIndex = mVertices.size();
	mVertices.insert(mVertices.end(), Vertex, Vertex + Count);
	return ReturningIndex;
}

template <typename T>
inline void FMesh<T>::AddTriangle(const uint32_t V1, const uint32_t V2, const uint32_t V3)
{
	mIndices.push_back(V1);
	mIndices.push_back(V2);
	mIndices.push_back(V3);
}

template <typename T>
inline void FMesh<T>::AddTriangle(const uint32_t* Indices, const uint32_t Count)
{
	mIndices.insert(mIndices.end(), Indices, Indices + (Count * 3));
}