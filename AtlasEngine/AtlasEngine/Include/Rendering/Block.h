#pragma once

#include "Math\Vector3.h"
#include <cstdint>

/**
* A voxel block.
*/
class FBlock
{
public:
	static const float BLOCK_SIZE; // Width of each voxel
	static const Vector3f Colors[]; // Color for each voxel type

public:
	// Each type of voxel
	enum BlockType : uint8_t
	{
		Grass,
		Dirt,
		Snow,
		Brick,
		Count
	};

public:
	FBlock(const BlockType Type = Grass, bool IsActive = false)
		: mIsActive(IsActive)
		, mType(Type){}

	~FBlock() = default;

	/**
	* Check if the block is active.
	*/
	bool IsActive() const { return mIsActive; }

	/**
	* Set the status of the block.
	*/
	void SetActive(bool IsActive) { mIsActive = IsActive; }

	/**
	* Get the block type.
	*/
	const BlockType& GetType() const { return mType; }

	/**
	* Set the block type.
	*/
	void SetType(const BlockType Type) { mType = Type; }

private:
	bool mIsActive{false};
	BlockType mType;
};


inline bool operator==(const FBlock& Lhs, const FBlock& Rhs)
{
	return (Lhs.GetType() == Rhs.GetType()) && (Lhs.IsActive() == Rhs.IsActive());
}

inline bool operator!=(const FBlock& Lhs, const FBlock& Rhs)
{
	return (Lhs.GetType() != Rhs.GetType()) || (Lhs.IsActive() != Rhs.IsActive());
}