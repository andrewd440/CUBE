#include "ChunkSystems\BlockTypes.h"

std::vector<Vector4f> FBlockTypes::mBlockTypes(std::numeric_limits<uint8_t>::max());

void FBlockTypes::AddBlock(const BlockID ID, const Vector4f& Color)
{
	mBlockTypes[ID] = Color;
}

const Vector4f& FBlockTypes::GetBlockColor(const BlockID ID)
{
	return mBlockTypes[ID];
}