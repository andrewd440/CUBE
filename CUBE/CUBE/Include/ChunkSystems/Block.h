#pragma once
#include <cstdint>

#include "Math\Vector3.h"
#include "BlockTypes.h"

/**
* A world voxel.
*/
struct FBlock
{
	static const float BLOCK_SIZE; // Width of each voxel
	static const uint8_t AIR_BLOCK_ID = 0;

	FBlock(const FBlockTypes::BlockID Type = AIR_BLOCK_ID)
		: ID(Type){}

	FBlockTypes::BlockID ID{AIR_BLOCK_ID};
};


inline bool operator==(const FBlock& Lhs, const FBlock& Rhs)
{
	return Lhs.ID == Rhs.ID;
}

inline bool operator!=(const FBlock& Lhs, const FBlock& Rhs)
{
	return Lhs.ID != Rhs.ID;
}