#pragma once

namespace FDebug
{
	struct DrawVertex;
}

namespace VertexTraits
{
	/////////////////////////////////////////////
	//////// FDebug::DrawVertex /////////////////
	/////////////////////////////////////////////

	template <>
	struct Attribute_Count < FDebug::DrawVertex >
	{
		const static uint32_t Count = 2;
	};

	const uint32_t GL_Attribute<FDebug::DrawVertex >::Position[] = { GLAttributePosition::Position, GLAttributePosition::Color };
	const uint32_t GL_Attribute<FDebug::DrawVertex >::ElementCount[] = { 3, 3 };
	const uint32_t GL_Attribute<FDebug::DrawVertex >::Type[] = { GL_FLOAT, GL_FLOAT };
	const bool GL_Attribute<FDebug::DrawVertex >::Normalized[] = { GL_FALSE, GL_FALSE };
	const uint32_t GL_Attribute<FDebug::DrawVertex >::Offset[] = { 0, 12 };
}