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
	static const Vector4f Colors[4]; // Color for each voxel type

	// Each type of voxel
	enum Type : uint8_t
	{
		Grass,
		Dirt,
		Snow,
		Brick,
		Count,
		None
	};

	FBlock(const Type Type = None)
		: BlockType(Type){}

	Type BlockType{None};
};


inline bool operator==(const FBlock& Lhs, const FBlock& Rhs)
{
	return Lhs.BlockType == Rhs.BlockType;
}

inline bool operator!=(const FBlock& Lhs, const FBlock& Rhs)
{
	return Lhs.BlockType != Rhs.BlockType;
}