#pragma once
#include "Rendering\GLUtils.h"
#include "Common.h"
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

inline uint32_t BMesh::VertexCountB() const
{
	return mVertexData.size();
}

inline void BMesh::AddIndicesB(const uint32_t* Indices, const uint32_t Count)
{
	mIndices.insert(mIndices.end(), Indices, Indices + Count);
}

inline void BMesh::SetUsageModeB(GLuint Usage)
{
	mUsageMode = Usage;
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
	if (IsActiveB())
		ActivateB();
}

template <typename T>
inline TMesh<T>::TMesh(TMesh<T>&& Other) _NOEXCEPT
	: BMesh(std::move(Other))
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
	if (IsActiveB())
		ActivateB();
	return *this;
}

template <typename T>
inline TMesh<T>& TMesh<T>::operator=(TMesh<T>&& Other) _NOEXCEPT
{
	BMesh::operator=(std::move(Other));
	return *this;
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
inline void TMesh<T>::AddIndices(const uint32_t* Indices, const uint32_t Count)
{
	AddIndicesB(Indices, Count);
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
inline uint32_t TMesh<T>::GetVertexCount() const
{
	return VertexCountB() / VertexSize;
}

template <typename T>
inline void TMesh<T>::MapAndActivate(const VertexType* VertexData, const uint32_t NumVertices, const uint32_t* IndexData, const uint32_t IndexSize)
{
	MapAndActivateB((void*)VertexData, NumVertices * sizeof(VertexType), IndexData, IndexSize);
}

//template <typename T>
//inline void TMesh<T>::MapIndexBuffer(uint32_t* Data, uint32_t Size)
//{
//	MapIndexBufferB((void*)Data, sizeof(uint32_t) * Size);
//}
//
//template <typename T>
//inline void TMesh<T>::MapVertexBuffer(VertexType* Data, uint32_t Size)
//{
//	MapVertexBufferB((void*)Data, sizeof(VertexType) * Size);
//}

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

template <typename T>
inline void TMesh<T>::SetUsageMode(GLuint Usage)
{
	SetUsageModeB(Usage);
}