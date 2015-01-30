#include "Rendering\Mesh.h"
#include "Rendering\GLUtils.h"
#include "Common.h"
#include "Rendering\VertexTraits.h"

BMesh::BMesh(const GLuint DrawMode, const uint32_t DefaultBufferSize )
	: mVertexData(DefaultBufferSize)
	, mIndices()
	, mVertexArray()
	, mIsActive(false)
	, mUsageMode(DrawMode)
{
	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(2, mBuffers);
}

BMesh::BMesh(const BMesh& Other)
	: mVertexData(Other.mVertexData)
	, mIndices(Other.mIndices)
	, mIsActive(Other.mIsActive)
{
	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(2, mBuffers);

	// If copy was active, activate this one.
	if (mIsActive)
		ActivateB();
}

BMesh::BMesh(BMesh&& Other)
	: mVertexData(std::move(Other.mVertexData))
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
	mIsActive = Other.mIsActive;

	if (mIsActive)
		ActivateB();
	else
		DeactivateB();

	return *this;
}

BMesh& BMesh::operator=(BMesh&& Other)
{
	// Delete buffers held
	glDeleteVertexArrays(1, &mVertexArray);
	glDeleteBuffers(2, mBuffers);

	// Steal data
	mVertexData = std::move(Other.mVertexData);
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

void BMesh::RenderB(const GLenum RenderMode)
{
	ASSERT(mIsActive);
	GLUtils::ArrayBinder VAOBinding(mVertexArray);
	glDrawElements(RenderMode, mIndices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
}

void BMesh::DeactivateB()
{
	//ASSERT(!mIsActive);
	if (!mIsActive)
		return;

	mIsActive = false;

	GLUtils::ArrayBinder VAOBinding(mVertexArray);

	// Clear vertex data from gpu
	GLUtils::BufferBinder<GL_ARRAY_BUFFER> VertexBinding(mBuffers[Vertex]);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, mUsageMode);

	// Clear index data from gpu
	GLUtils::BufferBinder<GL_ELEMENT_ARRAY_BUFFER> IndexBinding(mBuffers[Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, mUsageMode);
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
	GLUtils::BufferBinder<GL_ARRAY_BUFFER> VertexBinding(mBuffers[Vertex]);
	glBufferData(GL_ARRAY_BUFFER, mVertexData.size(), mVertexData.data(), mUsageMode);

	EnableAttributes();

	// Supply index data to gpu
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t), mIndices.data(), mUsageMode);
}