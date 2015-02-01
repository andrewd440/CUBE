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

	static FPoolAllocator<sizeof(FBlock) * BLOCKS_PER_CHUNK, 10000> ChunkAllocator;
	static FPoolAllocatorType<TMesh<FVoxelVertex>, 10000> MeshAllocator;

public:
	/**
	* Constructs chunk of voxels.
	*/
	FChunk();

	/**
	* Copy Ctor.
	* Copies pointer data into this mesh.
	* After the copy, both objects will refer to the same mesh and
	* block data.
	*/
	FChunk(const FChunk& Other);

	/**
	* Frees data back to respective pools.
	*/
	~FChunk();

	/**
	* Copy assignment.
	* Frees mesh and block data from this object and copies pointer 
	* data from Other into this object.
	* After the copy, both objects will refer to the same mesh and
	* block data.
	*/
	FChunk& operator=(const FChunk& Other);

	/**
	* Allocates and builds chunk data. Chunk meshes will still need to 
	* be built before rendering.
	* @param LowerLeftPosition - The starting world position of this chunk.
	*/
	void Load(const Vector3ui& LowerLeftPosition);

	/**
	* Frees block and mesh data.
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
	* Builds/Rebuilds this chunks' mesh.
	*/
	void RebuildMesh();

private:
	// Constants used for constructing quads with correct normals in GreedyMesh()
	static const uint32_t SOUTH = 0;
	static const uint32_t NORTH = 1;
	static const uint32_t EAST = 2;
	static const uint32_t WEST = 3;
	static const uint32_t TOP = 4;
	static const uint32_t BOTTOM = 5;

private:
	/**
	* Voxel mesh algorithm to minimize triangle count on chunk meshes.
	* Algorithm by Mikola Lysenko from http://0fps.net/2012/06/30/meshing-in-a-minecraft-game/
	*/
	void GreedyMesh();

	/**
	* Adds a quad from 4 vertices based on if the quad is backfaced, the direction of the surface,
	* and block type we are generating the quad for. Output is given through a given vertex and index
	* list.
	* @param Bottom left position
	* @param Top left position
	* @param Top right position
	* @param Bottom right position
	* @param IsBackFace - Used to specify vertex ordering.
	* @param Side - Direction the surface is facing
	* @param BlockType - The type of block the quad is used for.
	* @param VerticesOut - Location to place vertex data.
	* @param IndicesOut - Location to place index data.
	*/
	void AddQuad(	const Vector3f BottomLeft, 
					const Vector3f TopLeft, 
					const Vector3f TopRight, 
					const Vector3f BottomRight,
					bool IsBackface, 
					uint32_t Side, 
					const FBlock::BlockType BlockType,
					std::vector<FVoxelVertex>& VerticesOut,
					std::vector<uint32_t>& IndicesOut);

private:
	FBlock* mBlocks;
	TMesh<FVoxelVertex>* mMesh;
	bool mIsLoaded;
};