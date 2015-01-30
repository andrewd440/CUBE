#pragma once

#include <GL\glew.h>
#include <cstdint>

#include "..\Memory\PoolAllocator.h"
#include "..\Common.h"
#include "Block.h"
#include "Mesh.h"

/**
* Voxel vertex (P, N, C)
*/
struct FVoxelVertex
{
	Vector3f Position;
	Vector3f Normal;
	Vector3f Color;
};

/**
* Represents a 3D mesh of voxels of CHUNK_SIZE
* dimensions.
*/
class FChunk
{
public:
	// Dimensions of each chunk
	static const uint32_t CHUNK_SIZE = 16;
	static const uint32_t BLOCKS_PER_CHUNK = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

	static FPoolAllocator<sizeof(FBlock) * BLOCKS_PER_CHUNK, 1000> ChunkAllocator;
	static FPoolAllocatorType<TMesh<FVoxelVertex>, 1000> MeshAllocator;

public:
	/**
	* Constructs chunk of voxels.
	*/
	FChunk();

	~FChunk();

	/**
	* Allocates and builds chunk data. Chunk meshes will still need to 
	* be built before rendering.
	*/
	void Load(const Vector3i& LowerLeftPosition);

	/**
	* Frees chunk data and deactivates the chunk mesh.
	*/
	void Unload();

	/**
	* Checks if the chunk has been loaded.
	*/
	bool IsLoaded() const;

	/**
	* Renders active blocks in the chunk.
	*/
	void Render(const Vector3f& WorldPosition);

	/**
	* Builds/Rebuild this chunks mesh.
	*/
	void BuildMesh();

private:
	/**
	* Creates a adds a cube to this chunks mesh.
	* @param Position to add the cube.
	*/
	void CreateCube(const Vector3f& Position);

private:
	FBlock* mBlocks;
	TMesh<FVoxelVertex>* mMesh;
	bool mIsLoaded;
};