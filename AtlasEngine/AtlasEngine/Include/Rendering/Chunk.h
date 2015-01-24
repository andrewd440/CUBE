#pragma once

#include <GL\glew.h>
#include <cstdint>

#include "..\Common.h"
#include "Block.h"
#include "Mesh.h"

class FChunk
{
public:
	static const uint32_t CHUNK_SIZE = 16;

	FChunk();
	~FChunk();

	void Render();

	void CreateMesh();

private:
	void CreateCube(const Vector3f& Position);

private:
	FBlock Blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	FMesh mMesh;
};


inline void FChunk::Render()
{
	mMesh.Render();
}