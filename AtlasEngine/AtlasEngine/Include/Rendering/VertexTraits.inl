#pragma once

///////////////////////////////////////////////
///////// Debug Text Vertex ///////////////////
///////////////////////////////////////////////
struct FVoxelVertex;
struct FTextVertex;

namespace VertexTraits
{
	template <>
	struct Attribute_Count < FTextVertex >
	{
		const static uint32_t Count = 3;
	};

	const uint32_t GL_Attribute<FTextVertex>::Position[] = { GLAttributePosition::Position, GLAttributePosition::UV, GLAttributePosition::Color };
	const uint32_t GL_Attribute<FTextVertex>::ElementCount[] = { 3, 2, 4 };
	const uint32_t GL_Attribute<FTextVertex>::Type[] = { GL_FLOAT, GL_FLOAT, GL_FLOAT };
	const bool GL_Attribute<FTextVertex>::Normalized[] = { GL_FALSE, GL_FALSE, GL_FALSE };
	const uint32_t GL_Attribute<FTextVertex>::Offset[] = { 0, 12, 20 };


	/////////////////////////////////////////////
	//////// FVoxelVertex ///////////////////////
	/////////////////////////////////////////////

	template <>
	struct Attribute_Count < FVoxelVertex >
	{
		const static uint32_t Count = 3;
	};

	const uint32_t GL_Attribute<FVoxelVertex>::Position[] = { GLAttributePosition::Position, GLAttributePosition::Normal, GLAttributePosition::Color };
	const uint32_t GL_Attribute<FVoxelVertex>::ElementCount[] = { 3, 3, 3 };
	const uint32_t GL_Attribute<FVoxelVertex>::Type[] = { GL_FLOAT, GL_FLOAT, GL_FLOAT };
	const bool GL_Attribute<FVoxelVertex>::Normalized[] = { GL_FALSE, GL_FALSE, GL_FALSE };
	const uint32_t GL_Attribute<FVoxelVertex>::Offset[] = { 0, 12, 24 };
}