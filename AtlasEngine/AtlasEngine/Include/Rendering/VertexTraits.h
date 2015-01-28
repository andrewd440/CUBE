#pragma once
#include <cstdint>

namespace VertexTraits
{
	// OpenGL vertex data traits
	template <typename T>
	struct Attribute_Count
	{
		const static uint32_t Count = 0;
	};

	template <typename T>
	struct GL_Attribute
	{
		static const uint32_t Position[];
		static const uint32_t ElementCount[];
		static const uint32_t Type[];
		static const bool Normalized[];
		static const uint32_t Offset[];
	};
}

#include "VertexTraits.inl"