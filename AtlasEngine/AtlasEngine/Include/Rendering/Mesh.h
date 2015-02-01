#pragma once
#include <GL\glew.h>
#include <vector>
#include "..\Math\Vector3.h"

/**
* Base class for OpenGL indexed triangle mesh. 
* This class is not meant to be instantiated, only inherited. 
* A vertex array object, vertex buffer and index buffer is maintained within
* this object. This is used in conjunction with FMesh to minimize template code
* bloat when using different vertex types in mesh creation.
*/
class BMesh
{
public:
	/**
	* Defualt Ctor.
	* @param UsageMode - The usage of the OpenGL vertex buffer. (i.e. GL_STATIC_DRAW)
	* @param DefaultBufferSize - The estimated size, in bytes, of vertex data
	*							that will be used for this mesh.
	*/
	BMesh(const GLuint UsageMode, const uint32_t DefaultBufferSize = 0);

	/**
	* Copy Ctor.
	* Copies vertex and index data from Other
	* and opens new OpenGL buffers for its data.
	*/
	BMesh(const BMesh& Other);

	/**
	* Move Ctor.
	* Steals vertex data, index data and OpenGL buffers
	* from Other.
	*/
	BMesh(BMesh&& Other);

	/**
	* Deletes buffers held by this object.
	*/
	~BMesh();

	/**
	* Copies vertex and index data from Other
	*/
	BMesh& operator=(const BMesh& Other);

	/**
	* Deletes OpenGL buffers held by this object and
	* vertex data, index data and OpenGL buffers
	* from Other.
	*/
	BMesh& operator=(BMesh&& Other);

	/**
	* Renders vertex data held by this objects' buffers.
	* Can be supplied with different OpenGL render modes.
	*/
	void RenderB(const GLenum RenderMode = GL_TRIANGLES);

	/**
	* Adds a vertex to the mesh.
	* @param Vertex to add.
	* @return ID of the added vertex. This is used to add
	*	triangles to the mesh with AddTriangle()
	*/
	uint32_t AddVertexB(const uint8_t* Vertex, const uint32_t VertexSize);

	/**
	* Adds a list of vertices to the mesh.
	* @param Vertex - The vertex list.
	* @param Count - The number of vertices in this list.
	* @return The ID of the first vertex added.
	*/
	uint32_t AddVertexB(const uint8_t* Vertex, const uint32_t VertexSize, const uint32_t Count);

	/**
	* Adds a triangle to the mesh. Vertices must be in CCW order.
	* @param V1 - First vertex ID.
	* @param V2 - Second vertex ID.
	* @param V3 - Third vertex ID.
	*/
	void AddTriangleB(const uint32_t V1, const uint32_t V2, const uint32_t V3);

	/**
	* Adds a list of triangles to the mesh. Vertices must be in CCW order.
	* @param Indices - The list of vertex indices for each triangle.
	* @param Count - Number of triangles to add.
	*/
	void AddIndicesB(const uint32_t* Indices, const uint32_t Count);

	/**
	* Clears data held internally by this object and empties
	* OpenGL buffer objects held by this object.
	*/
	void DeactivateB();

	/**
	* Allocates OpenGL buffer objects and sends vertex data
	* to the GPU.
	*/
	void ActivateB();

	/**
	* Copies and activates vertex and index data directly into their respective 
	* buffer held by this mesh.
	* @param VertexData - Vertices to be mapped.
	* @param VertexSize - The size, in bytes, of the vertex data.
	* @param IndexData - Indices to be mapped.
	* @param IndexSize - The size, in bytes, of the index data.
	*/
	void MapAndActivateB(void* VertexData, uint32_t VertexSize, uint32_t* IndexData, uint32_t IndexSize);

	/**
	* Clears all vertex and index data held locally be this
	* object.
	*/
	void ClearDataB();

	/**
	* Gets the status of the object.
	*/
	bool IsActiveB() const;

	/**
	* Get the index count of the mesh.
	*/
	uint32_t GetIndexCountB() const;

	/**
	* Get the triangle count of the mesh.
	*/
	uint32_t TriangleCountB() const;

	/**
	* Copies data directly into the index buffer held
	* by this mesh. 
	* @param Data to be mapped.
	* @param Size - The size, in bytes, of the data.
	*/
	//void MapIndexBufferB(void* Data, uint32_t Size);

	/**
	* Copies data directly into the vertex buffer held
	* by this mesh.
	* @param Data to be mapped.
	* @param Size - The size, in bytes, of the data.
	*/
	//void MapVertexBufferB(void* Data, uint32_t Size);

	/**
	* Enables all OpenGL attributes for this mesh.
	*/
	virtual void EnableAttributes() = 0;

private:
	enum BufferType
	{
		Vertex,
		Index,
	};

private:
	std::vector<uint8_t> mVertexData;
	std::vector<uint32_t> mIndices;

	// GL buffers held by this object
	GLuint mVertexArray;
	GLuint mBuffers[2];

	// Buffer usage mode
	GLuint mUsageMode;

	// Number of indices currently in the index buffer
	uint32_t mIndexCount;

	bool mIsActive;
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// FMesh ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
/**
* Class for creating an OpenGL indexed triangle mesh.
* Triangle data can be added to the mesh before
* sending that data to the GPU. Once all data has
* been feed into this object, it then needs to be activate
* with Activate(), which sends it's information to the GPU,
* before it can be rendered.
*/
class TMesh : private BMesh
{
public:
	using VertexType = T;
	static const uint32_t VertexSize = sizeof(T);

public:
	/**
	* Defualt Ctor.
	* @param UsageMode - The usage of the OpenGL vertex buffer. (i.e. GL_STATIC_DRAW)
	* @param DefaultBufferSize - The estimated size, in bytes, of vertex data
	*							that will be used for this mesh.
	*/
	TMesh(const GLuint UsageMode, const uint32_t DefaultBufferSize = 0);

	/**
	* Copy Ctor.
	* Copies vertex and index data from Other
	* and opens new OpenGL buffers for its data.
	*/
	TMesh(const TMesh& Other);

	/**
	* Move Ctor.
	* Steals vertex data, index data and OpenGL buffers
	* from Other.
	*/
	TMesh(TMesh&& Other);

	/**
	* Deletes buffers held by this object.
	*/
	~TMesh();

	/**
	* Copies vertex and index data from Other
	*/
	TMesh& operator=(const TMesh& Other);

	/**
	* Deletes OpenGL buffers held by this object and
	* vertex data, index data and OpenGL buffers
	* from Other.
	*/
	TMesh& operator=(TMesh&& Other);

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
	* @param Count - Number of indices to add.
	*/
	void AddIndices(const uint32_t* Indices, const uint32_t Count);

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
	* Clears all vertex and index data held locally be this
	* object.
	*/
	void ClearData();

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

	/**
	* Copies and activates vertex and index data directly into their respective
	* buffer held by this mesh.
	* @param VertexData - Vertices to be mapped.
	* @param VertexSize - Number of vertices to copy.
	* @param IndexData - Indices to be mapped.
	* @param IndexSize - Number of indices to copy.
	*/
	void MapAndActivate(VertexType* VertexData, uint32_t VertexSize, uint32_t* IndexData, uint32_t IndexSize);

	/**
	* Copies data directly into the index buffer held
	* by this mesh.
	* @param Data to be mapped.
	* @param Size - Number of indices to copy.
	*/
	//void MapIndexBuffer(uint32_t* Data, uint32_t Size);

	/**
	* Copies data directly into the vertex buffer held
	* by this mesh.
	* @param Data to be mapped.
	* @param Size - Number of vertices to copy.
	*/
	//void MapVertexBuffer(VertexType* Data, uint32_t Size);

private:
	/**
	* Enables OpenGL vertex attributes from this mesh.
	*/
	void EnableAttributes();
};

#include "Mesh.inl"