#include "Rendering\Mesh.h"
#include "Rendering\GLUtils.h"
#include "Common.h"
#include "Rendering\VertexTraits.h"
#include "SFML\Window\Context.hpp"
#include "Components\MeshComponent.h"

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
	ASSERT(!mIsActive);

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

//void BMesh::MapIndexBufferB(void* Data, uint32_t Size)
//{
//	GLUtils::ArrayBinder VAOBinding(mVertexArray);
//	void* Ptr = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
//	memcpy(Ptr, Data, Size);
//	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
//}
//
//void BMesh::MapVertexBufferB(void* Data, uint32_t Size)
//{
//	GLUtils::ArrayBinder VAOBinding(mVertexArray);
//	void* Ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
//	memcpy(Ptr, Data, Size);
//	glUnmapBuffer(GL_ARRAY_BUFFER);
//}

//template <>
//bool TMesh<MeshVertex>::LoadModel(const wchar_t* ModelFilepath)
//{
//	std::ifstream ModelFile(ModelFilepath);
//
//	if (ModelFile.fail())
//	{
//		std::cerr << "Failed to open FMesh model filename: "
//			<< ModelFilepath;
//		return false;
//	}
//
//	std::string FileLine;
//	int32_t VertexCount = 0;
//	int32_t IndexCount = 0;
//
//	// Get header data
//	while (getline(ModelFile, FileLine))
//	{
//		if (FileLine.substr(0, 14) == "element vertex")
//		{
//			const std::string VertexCountString = FileLine.substr(15, FileLine.find(' '));
//			VertexCount = atoi(VertexCountString.c_str());
//		}
//		else if (FileLine.substr(0, 12) == "element face")
//		{
//			const std::string ElementCountString = FileLine.substr(13, FileLine.find(' '));
//			IndexCount = atoi(ElementCountString.c_str());
//		}
//		else if (FileLine == "end_header")
//		{
//			break;
//		}
//	}
//
//	std::vector<MeshVertex> Vertices(VertexCount);
//	
//	// Vertex data
//	for (uint32_t i = 0; i < VertexCount; i++)
//	{
//		getline(ModelFile, FileLine);
//		const std::string XString = FileLine.substr(0, FileLine.find(' '));
//		const float X = (float)atof(XString.c_str());
//
//		FileLine = FileLine.substr(XString.length() + 1);
//		const std::string YString = FileLine.substr(0, FileLine.find(' '));
//		const float Y = (float)atof(YString.c_str());
//
//		FileLine = FileLine.substr(YString.length() + 1);
//		const std::string ZString = FileLine.substr(0, FileLine.find(' '));
//		const float Z = (float)atof(ZString.c_str());
//
//		FileLine = FileLine.substr(ZString.length() + 1);
//		const std::string XnString = FileLine.substr(0, FileLine.find(' '));
//		const float Xn = (float)atof(XnString.c_str());
//
//		FileLine = FileLine.substr(XnString.length() + 1);
//		const std::string YnString = FileLine.substr(0, FileLine.find(' '));
//		const float Yn = (float)atof(YnString.c_str());
//
//		FileLine = FileLine.substr(YnString.length() + 1);
//		const std::string ZnString = FileLine.substr(0, FileLine.find(' '));
//		const float Zn = (float)atof(ZnString.c_str());
//
//		Vertices[i] = MeshVertex{ Vector4f{ X, Y, Z, 1.0f }, Vector3f{ Xn, Yn, Zn }, Vector3f{ 1, 1, 1 } };
//	}
//
//	std::vector<uint32_t> Indices;
//
//	// Index data
//	for (uint32_t i = 0; i < IndexCount; i++)
//	{
//		uint32_t QuadIndices[4];
//
//		getline(ModelFile, FileLine);
//		FileLine = FileLine.substr(2);
//
//		const std::string String0 = FileLine.substr(0, FileLine.find(' '));
//		QuadIndices[0] = (uint32_t)atoi(String0.c_str());
//
//		FileLine = FileLine.substr(String0.length() + 1);
//		const std::string String1 = FileLine.substr(0, FileLine.find(' '));
//		QuadIndices[1] = (uint32_t)atoi(String1.c_str());
//
//		FileLine = FileLine.substr(String1.length() + 1);
//		const std::string String2 = FileLine.substr(0, FileLine.find(' '));
//		QuadIndices[2] = (uint32_t)atoi(String2.c_str());
//
//		FileLine = FileLine.substr(String2.length() + 1);
//		const std::string String3 = FileLine.substr(0, FileLine.find(' '));
//		QuadIndices[3] = (uint32_t)atoi(String3.c_str());
//
//		Indices.insert(Indices.end(), { QuadIndices[0], QuadIndices[1], QuadIndices[2], QuadIndices[2], QuadIndices[3], QuadIndices[0] });
//	}
//	ModelFile.close();
//
//	AddVertex(Vertices.data(), VertexCount);
//	AddIndices(Indices.data(), Indices.size());
//	Activate();
//}