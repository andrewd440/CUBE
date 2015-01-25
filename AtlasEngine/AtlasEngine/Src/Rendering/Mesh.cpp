#include "..\..\Include\Rendering\Mesh.h"
#include "..\..\Include\Rendering\GLUtils.h"
#include "..\..\Include\Common.h"

FMesh::FMesh()
	: mVertices()
	, mIndices()
	, mVertexArray()
	, mIsActive(false)
{
	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(2, mBuffers);
}

FMesh::FMesh(const FMesh& Other)
	: mVertices(Other.mVertices)
	, mIndices(Other.mIndices)
	, mIsActive(Other.mIsActive)
{
	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(2, mBuffers);

	if (mIsActive)
		Activate();
}

FMesh::FMesh(FMesh&& Other)
	: mVertices(std::move(Other.mVertices))
	, mIndices(std::move(Other.mIndices))
	, mVertexArray(Other.mVertexArray)
	, mIsActive(Other.mIsActive)
{
	// Copy buffers
	FOR(i, 2)
		mBuffers[i] = Other.mBuffers[i];

	// Remove buffers from Other
	Other.mVertexArray = 0;
	Other.mBuffers[0] = Other.mBuffers[1] = 0;
	Other.mIsActive = false;
}

FMesh::~FMesh()
{
	glDeleteVertexArrays(1, &mVertexArray);
	glDeleteBuffers(2, mBuffers);
}

FMesh& FMesh::operator=(const FMesh& Other)
{
	// Copy vertex data
	mVertices = Other.mVertices;
	mIndices = Other.mIndices;
	mIsActive = Other.mIsActive;

	if (mIsActive)
		Activate();
	else
		Deactivate();

	return *this;
}

FMesh& FMesh::operator=(FMesh&& Other)
{
	// Delete buffers held
	glDeleteVertexArrays(1, &mVertexArray);
	glDeleteBuffers(2, mBuffers);

	// Steal data
	mVertices = std::move(Other.mVertices);
	mIndices = std::move(Other.mIndices);
	mVertexArray = Other.mVertexArray;
	mIsActive = Other.mIsActive;

	// Copy buffers
	FOR(i, 2)
		mBuffers[i] = Other.mBuffers[i];

	// Remove buffers from Other
	Other.mVertexArray = 0;
	Other.mBuffers[0] = Other.mBuffers[1] = 0;
	Other.mIsActive = false;

	return *this;
}

void FMesh::Render(const GLenum RenderMode)
{
	ASSERT(mIsActive);
	GLUtils::ArrayBinder VAOBinding(mVertexArray);
	glDrawElements(RenderMode, mIndices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
}

void FMesh::Deactivate()
{
	if (!mIsActive)
		return;

	mIsActive = false;

	// Free data
	mIndices.clear();
	mVertices.clear();

	GLUtils::ArrayBinder VAOBinding(mVertexArray);

	// Clear vertex data from gpu
	GLUtils::BufferBinder<GL_ARRAY_BUFFER> VertexBinding(mBuffers[Vertex]);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	// Clear index data from gpu
	GLUtils::BufferBinder<GL_ELEMENT_ARRAY_BUFFER> IndexBinding(mBuffers[Vertex]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
}

void FMesh::Activate()
{
	mIsActive = true;

	// Clear unused space
	mVertices.shrink_to_fit();
	mIndices.shrink_to_fit();

	GLUtils::ArrayBinder VAOBinding(mVertexArray);

	// Supply vertex data to gpu
	GLUtils::BufferBinder<GL_ARRAY_BUFFER> VertexBinding(mBuffers[Vertex]);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(FVoxelVertex), mVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(GLAttributePosition::Position, 3, GL_FLOAT, GL_FALSE, sizeof(FVoxelVertex), BUFFER_OFFSET(offsetof(FVoxelVertex, Position)));
	glEnableVertexAttribArray(GLAttributePosition::Position);

	glVertexAttribPointer(GLAttributePosition::Normal, 3, GL_FLOAT, GL_FALSE, sizeof(FVoxelVertex), BUFFER_OFFSET(offsetof(FVoxelVertex, Normal)));
	glEnableVertexAttribArray(GLAttributePosition::Normal);

	glVertexAttribPointer(GLAttributePosition::Color, 3, GL_FLOAT, GL_FALSE, sizeof(FVoxelVertex), BUFFER_OFFSET(offsetof(FVoxelVertex, Color)));
	glEnableVertexAttribArray(GLAttributePosition::Color);

	// Supply index data to gpu
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t), mIndices.data(), GL_STATIC_DRAW);
}