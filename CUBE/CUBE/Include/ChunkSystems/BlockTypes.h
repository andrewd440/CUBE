#pragma once

#include <vector>
#include <cstdint>

#include "Math\Vector4.h"

/**
* This class is used to bind a block type with an ID. The
* ID type is of FBlockTypes::BlockID. The maximum amount of
* unique block types is the max numeric size of the type used 
* to store block IDs. You can overwrite block types by adding a
* block type to a previously used ID.
*/
class FBlockTypes
{
public:
	using BlockID = uint8_t;

public:
	FBlockTypes() = delete;

	static void AddBlock(const BlockID ID, const Vector4f& Color);
	static const Vector4f& GetBlockColor(const BlockID ID);

private:
	friend class FRenderSystem;
	static std::vector<Vector4f> mBlockTypes;
};