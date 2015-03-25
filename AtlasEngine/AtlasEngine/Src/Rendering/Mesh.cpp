#include "Rendering\Mesh.h"
#include "Rendering\GLUtils.h"
#include "Common.h"
#include "Rendering\VertexTraits.h"
#include "SFML\Window\Context.hpp"
#include "Components\MeshComponent.h"
#include "tinyobjloader\tiny_obj_loader.h"
#include <fstream>

BMesh::BMesh(const GLuint DrawMode, const uint32_t DefaultBufferSize )
	: mVertexData(DefaultBufferSize)
	, mIndices()
	, mVertexArray()
	, mUsageMode(DrawMode)
	, mIndexCount(0)
	, mIsActive(false)
{
	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(2, mBuffers);
}

BMesh::BMesh(const BMesh& Other)
	: mVertexData(Other.mVertexData)
	, mIndices(Other.mIndices)
	, mUsageMode(Other.mUsageMode)
	, mIndexCount(Other.mIndexCount)
	, mIsActive(Other.mIsActive)
{
	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(2, mBuffers);
}

BMesh::BMesh(BMesh&& Other) _NOEXCEPT
	: mVertexData(std::move(Other.mVertexData))
	, mIndices(std::move(Other.mIndices))
	, mVertexArray(Other.mVertexArray)
	, mUsageMode(Other.mUsageMode)
	, mIndexCount(Other.mIndexCount)
	, mIsActive(Other.mIsActive)
{
	// Copy buffers
	FOR(i, 2)
		mBuffers[i] = Other.mBuffers[i];

	// Remove buffers from Other
	Other.mVertexArray = 0;
	Other.mBuffers[0] = Other.mBuffers[1] = 0;

	// Set invalid data
	Other.mIndexCount = 0;
	Other.mUsageMode = 0;
	Other.mIsActive = false;
}

BMesh::~BMesh()
{
	glDeleteVertexArrays(1, &mVertexArray);
	glDeleteBuffers(2, mBuffers);
}

BMesh& BMesh::operator=(const BMesh& Other)
{
	// Copy vertex data
	mVertexData = Other.mVertexData;
	mIndices = Other.mIndices;
	mUsageMode = Other.mUsageMode;
	mIndexCount = Other.mIndexCount;
	mIsActive = Other.mIsActive;

	return *this;
}

BMesh& BMesh::operator=(BMesh&& Other) _NOEXCEPT
{
	// Delete buffers held
	glDeleteVertexArrays(1, &mVertexArray);
	glDeleteBuffers(2, mBuffers);

	// Steal data
	mVertexData = std::move(Other.mVertexData);
	mIndices = std::move(Other.mIndices);
	mVertexArray = Other.mVertexArray;
	mUsageMode = Other.mUsageMode;
	mIndexCount = Other.mIndexCount;
	mIsActive = Other.mIsActive;

	// Copy buffers
	FOR(i, 2)
		mBuffers[i] = Other.mBuffers[i];

	// Remove buffers from Other
	Other.mVertexArray = 0;
	Other.mBuffers[0] = Other.mBuffers[1] = 0;
	
	// Set invalid data
	Other.mIndexCount = 0;
	Other.mUsageMode = 0;
	Other.mIsActive = false;

	return *this;
}

void BMesh::RenderB(const GLenum RenderMode)
{
	ASSERT(mIsActive);
	GLUtils::ArrayBinder VAOBinding(mVertexArray);

	glDrawElements(RenderMode, mIndexCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
}

void BMesh::DeactivateB()
{
	mIsActive = false;

	GLUtils::ArrayBinder VAOBinding(mVertexArray);

	// Clear vertex data from gpu
	GLUtils::BufferBinder<GL_ARRAY_BUFFER> VertexBinding(mBuffers[Vertex]);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, mUsageMode);

	// Clear index data from gpu
	GLUtils::BufferBinder<GL_ELEMENT_ARRAY_BUFFER> IndexBinding(mBuffers[Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, mUsageMode);

	mIndexCount = 0;
}

void BMesh::ClearDataB()
{
	// Clear local data
	mVertexData.clear();
	mIndices.clear();
}

void BMesh::ActivateB()
{
	mIsActive = true;

	GLUtils::ArrayBinder VAOBinding(mVertexArray);

	// Supply vertex data to gpu
	glBindBuffer(GL_ARRAY_BUFFER, mBuffers[Vertex]);
	glBufferData(GL_ARRAY_BUFFER, mVertexData.size(), mVertexData.data(), mUsageMode);

	EnableAttributes();

	// Supply index data to gpu
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t), mIndices.data(), mUsageMode);

	// Save index count separately incase local data is cleared.
	mIndexCount = mIndices.size();
}

uint32_t BMesh::AddVertexB(const uint8_t* Vertex, const uint32_t VertexSize)
{
	uint32_t ReturningIndex = mVertexData.size();
	mVertexData.insert(mVertexData.end(), Vertex, Vertex + VertexSize);
	return ReturningIndex;
}

uint32_t BMesh::AddVertexB(const uint8_t* Vertex, const uint32_t VertexSize, const uint32_t Count)
{
	uint32_t ReturningIndex = mVertexData.size();
	mVertexData.insert(mVertexData.end(), Vertex, Vertex + (VertexSize * Count));
	return ReturningIndex;
}

void BMesh::AddTriangleB(const uint32_t V1, const uint32_t V2, const uint32_t V3)
{
	mIndices.push_back(V1);
	mIndices.push_back(V2);
	mIndices.push_back(V3);
}

void BMesh::MapAndActivateB(const void* VertexData, const uint32_t VertexSize, const uint32_t* IndexData, const uint32_t IndexSize)
{
	mIsActive = true;

	GLUtils::ArrayBinder VAOBinding(mVertexArray);

	// Allocate the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, mBuffers[Vertex]);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, nullptr, mUsageMode);

	// Map data to the vertex buffer
	void* VertexPtr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(VertexPtr, VertexData, VertexSize);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	EnableAttributes();

	// Allocate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * IndexSize, nullptr, mUsageMode);

	// Map data to the index buffer
	void* ElementPtr = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ElementPtr, IndexData, sizeof(uint32_t) * IndexSize);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	mIndexCount = IndexSize;
}

template <>
bool TMesh<MeshVertex>::LoadModel(const char* ModelFilepath)
{
	std::vector<MeshVertex> Vertices;
	std::vector<uint32_t> Indices;

	std::vector<tinyobj::shape_t> Shapes;
	std::vector<tinyobj::material_t> Materials;
	std::string Error = tinyobj::LoadObj(Shapes, Materials, ModelFilepath);

	if (!Error.empty()) 
	{
		std::cerr << Error << std::endl;
		return false;
	}

	for (uint32_t s = 0; s < Shapes.size(); s++)
	{
		tinyobj::mesh_t& Mesh = Shapes[s].mesh;

		const uint32_t VerticesSize = Vertices.size();
		for (uint32_t i = 0; i < Mesh.indices.size(); i++)
		{
			Indices.push_back(Mesh.indices[i] + VerticesSize);
		}

		for (uint32_t i = 0; i < Mesh.positions.size(); i+=3)
		{
			const Vector4f P{ Mesh.positions[i + 0], Mesh.positions[i + 1], Mesh.positions[i + 2], 1 };
			const Vector3f N{ Mesh.normals[i + 0], Mesh.normals[i + 1], Mesh.normals[i + 2] };

			Vertices.push_back(MeshVertex{ P, N, Vector3f{} });
		}

		for (uint32_t i = 0; i < Mesh.material_ids.size(); i++)
		{
			uint32_t MatID = Mesh.material_ids[i];
			Vertices[Indices[VerticesSize + i * 3]].Color = Vector3f{ Materials[MatID].diffuse[0], Materials[MatID].diffuse[1], Materials[MatID].diffuse[2] };
			Vertices[Indices[VerticesSize + i * 3 + 1]].Color = Vector3f{ Materials[MatID].diffuse[0], Materials[MatID].diffuse[1], Materials[MatID].diffuse[2] };
			Vertices[Indices[VerticesSize + i * 3 + 2]].Color = Vector3f{ Materials[MatID].diffuse[0], Materials[MatID].diffuse[1], Materials[MatID].diffuse[2] };
		}

	}

	AddVertex(Vertices.data(), Vertices.size());
	AddIndices(Indices.data(), Indices.size());
	Activate();

	return true;
}