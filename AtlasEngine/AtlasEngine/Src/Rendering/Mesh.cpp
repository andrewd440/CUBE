#include "..\..\Include\Rendering\GLUtils.h"
#include "..\..\Include\Common.h"
#include "..\..\Include\VertexTraits.h"

template <typename T>
FMesh<T>::FMesh()
	: mVertices()
	, mIndices()
	, mVertexArray()
	, mIsActive(false)
{
	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(2, mBuffers);
}

template <typename T>
FMesh<T>::FMesh(const FMesh<T>& Other)
	: mVertices(Other.mVertices)
	, mIndices(Other.mIndices)
	, mIsActive(Other.mIsActive)
{
	glGenVertexArrays(1, &mVertexArray);
	glGenBuffers(2, mBuffers);

	if (mIsActive)
		Activate();
}

template <typename T>
FMesh<T>::FMesh(FMesh<T>&& Other)
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

template <typename T>
FMesh<T>::~FMesh()
{
	glDeleteVertexArrays(1, &mVertexArray);
	glDeleteBuffers(2, mBuffers);
}

template <typename T>
FMesh<T>& FMesh<T>::operator=(const FMesh<T>& Other)
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

template <typename T>
FMesh<T>& FMesh<T>::operator=(FMesh<T>&& Other)
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

template <typename T>
void FMesh<T>::Render(const GLenum RenderMode)
{
	ASSERT(mIsActive);
	GLUtils::ArrayBinder VAOBinding(mVertexArray);
	glDrawElements(RenderMode, mIndices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
}

template <typename T>
void FMesh<T>::Deactivate()
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

template <typename T>
void FMesh<T>::Activate()
{
	mIsActive = true;

	GLUtils::ArrayBinder VAOBinding(mVertexArray);

	// Supply vertex data to gpu
	GLUtils::BufferBinder<GL_ARRAY_BUFFER> VertexBinding(mBuffers[Vertex]);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(VertexType), mVertices.data(), GL_STATIC_DRAW);

	using namespace VertexTraits;
	for (uint32_t i = 0; i < Attribute_Count<VertexType>::Count; i++)
	{
		glVertexAttribPointer(GL_Attribute<VertexType>::Position[i],
			GL_Attribute<VertexType>::ElementCount[i],
			GL_Attribute<VertexType>::Type[i],
			GL_Attribute<VertexType>::Normalized[i],
			sizeof(VertexType),
			BUFFER_OFFSET(GL_Attribute<VertexType>::Offset[i]));
		glEnableVertexAttribArray(GL_Attribute<VertexType>::Position[i]);
	}

	// Supply index data to gpu
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t), mIndices.data(), GL_STATIC_DRAW);


	// Clear local data
	mVertices.clear();
	mIndices.clear();
}