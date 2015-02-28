#include "..\..\Include\Rendering\Block.h"

const float FBlock::BLOCK_SIZE = 1.0f;

const Vector3f FBlock::Colors[BlockType::Count] =
{
	Vector3f{ 0.1f, 0.35f, 0.15f },		// Grass
	Vector3f{ 0.47f, 0.28f, 0.0f },		// Dirt
	Vector3f{ 1.0f, 0.98f, 0.98f },		// Snow
	Vector3f{ 0.59f, 0.086f, 0.043f },	// Brick
};