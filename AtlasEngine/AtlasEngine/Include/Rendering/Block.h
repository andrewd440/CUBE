#pragma once

#include "Math\Vector3.h"
#include <cstdint>

/**
* A voxel block.
*/
struct FBlock
{
public:
	static const float BLOCK_SIZE; // Width of each voxel
	static const Vector3f Colors[]; // Color for each voxel type

	// Each type of voxel
	enum BlockType : uint8_t
	{
		Grass,
		Dirt,
		Snow,
		Brick,
		Count,
		None
	};

	FBlock(const BlockType BlockType = None)
		: Type(BlockType){}

	BlockType Type{None};
};


inline bool operator==(const FBlock& Lhs, const FBlock& Rhs)
{
	return Lhs.Type == Rhs.Type;
}

inline bool operator!=(const FBlock& Lhs, const FBlock& Rhs)
{
	return Lhs.Type != Rhs.Type;
}