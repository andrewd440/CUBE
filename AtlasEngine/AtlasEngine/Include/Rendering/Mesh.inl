#pragma once
#include "..\..\Include\Rendering\GLUtils.h"
#include "..\..\Include\Common.h"
#include "VertexTraits.h"

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Inlined BMesh ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

inline bool BMesh::IsActiveB() const
{
	return mIsActive;
}

inline uint32_t BMesh::GetIndexCountB() const
{
	return mIndices.size();
}

inline uint32_t BMesh::TriangleCountB() const
{
	return mVertexData.size() / 3;
}

inline uint32_t BMesh::AddVertexB(const uint8_t* Vertex, const uint32_t VertexSize)
{
	uint32_t ReturningIndex = mVertexData.size();
	mVertexData.insert(mVertexData.end(), Vertex, Vertex + VertexSize);
	return ReturningIndex;
}

inline uint32_t BMesh::AddVertexB(const uint8_t* Vertex, const uint32_t VertexSize, const uint32_t Count)
{
	uint32_t ReturningIndex = mVertexData.size();
	mVertexData.insert(mVertexData.end(), Vertex, Vertex + (VertexSize * Count));
	return ReturningIndex;
}

inline void BMesh::AddTriangleB(const uint32_t V1, const uint32_t V2, const uint32_t V3)
{
	mIndices.push_back(V1);
	mIndices.push_back(V2);
	mIndices.push_back(V3);
}

inline void BMesh::AddTriangleB(const uint32_t* Indices, const uint32_t Count)
{
	mIndices.insert(mIndices.end(), Indices, Indices + (Count * 3));
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Inlined FMesh ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline TMesh<T>::TMesh(const GLuint UsageMode, const uint32_t DefaultBufferSize = 0)
	: BMesh(UsageMode, sizeof(T) * DefaultBufferSize)
{
}

template <typename T>
inline TMesh<T>::TMesh(const TMesh<T>& Other)
	: BMesh(Other)
{

}

template <typename T>
inline TMesh<T>::TMesh(TMesh<T>&& Other)
	: BMesh(Other)
{

}

template <typename T>
inline TMesh<T>::~TMesh()
{
}

template <typename T>
inline TMesh<T>& TMesh<T>::operator=(const TMesh<T>& Other)
{
	BMesh::operator=(Other);
}

template <typename T>
inline TMesh<T>& TMesh<T>::operator=(TMesh<T>&& Other)
{
	BMesh::operator=(Other);
}

template <typename T>
inline void TMesh<T>::Render(const GLenum RenderMode)
{
	RenderB(RenderMode);
}

template <typename T>
inline uint32_t TMesh<T>::AddVertex(const VertexType& Vertex)
{
	uint32_t ByteIndex = AddVertexB((uint8_t*)&Vertex, VertexSize);
	return ByteIndex / VertexSize;
}

template <typename T>
inline uint32_t TMesh<T>::AddVertex(const VertexType* Vertex, const uint32_t Count)
{
	uint32_t ByteIndex = AddVertexB((uint8_t*)Vertex, VertexSize, Count);
	return ByteIndex / VertexSize;
}

template <typename T>
inline void TMesh<T>::AddTriangle(const uint32_t V1, const uint32_t V2, const uint32_t V3)
{
	AddTriangleB(V1, V2, V3);
}

template <typename T>
inline void TMesh<T>::AddTriangle(const uint32_t* Indices, const uint32_t Count)
{
	AddTriangleB(Indices, Count);
}

template <typename T>
inline void TMesh<T>::Deactivate()
{
	DeactivateB();
}

template <typename T>
inline void TMesh<T>::Activate()
{
	ActivateB();
}

template <typename T>
void TMesh<T>::ClearData()
{
	ClearDataB();
}

template <typename T>
inline bool TMesh<T>::IsActive() const
{
	return IsActiveB();
}

template <typename T>
inline uint32_t TMesh<T>::GetIndexCount() const
{
	return GetIndexCountB();
}

template <typename T>
inline uint32_t TMesh<T>::TriangleCount() const
{
	return TriangleCountB() / VertexSize;
}

template <typename T>
inline void TMesh<T>::EnableAttributes()
{
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
}
