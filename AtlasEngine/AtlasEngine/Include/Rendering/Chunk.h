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
	Vector4f PositionAO;
	Vector3f Normal;
	Vector3f Color;
};

// Voxel Vertex traits
template <>
struct VertexTraits::Attribute_Count < FVoxelVertex >
{
	const static uint32_t Count = 3;
};

const uint32_t VertexTraits::GL_Attribute<FVoxelVertex>::Position[] = { GLAttributePosition::Position, GLAttributePosition::Normal, GLAttributePosition::Color };
const uint32_t VertexTraits::GL_Attribute<FVoxelVertex>::ElementCount[] = { 4, 3, 3 };
const uint32_t VertexTraits::GL_Attribute<FVoxelVertex>::Type[] = { GL_FLOAT, GL_FLOAT, GL_FLOAT };
const bool VertexTraits::GL_Attribute<FVoxelVertex>::Normalized[] = { GL_FALSE, GL_FALSE, GL_FALSE };
const uint32_t VertexTraits::GL_Attribute<FVoxelVertex>::Offset[] = { 0, 16, 28 };

class FChunkManager;

/**
* Represents a 3D mesh of voxels of CHUNK_SIZE
* dimensions.
*/
class FChunk
{
public:
	// Dimensions of each chunk
	static const int32_t CHUNK_SIZE = 32;
	static const int32_t BLOCKS_PER_CHUNK = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

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

	void SetChunkManager(FChunkManager* NewManager);

	/**
	* Allocates and builds chunk data. Chunk meshes will still need to 
	* be built before rendering.
	* @param LowerLeftPosition - The starting world position of this chunk.
	*/
	void Load(const Vector3i& LowerLeftPosition);

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
	void Render(const GLenum RenderMode = GL_TRIANGLES);

	/**
	* Builds/Rebuilds this chunks' mesh.
	*/
	void RebuildMesh();

	/**
	* Set a block in the chunk at a specific position.
	*/
	void SetBlock(const Vector3i& Position, FBlock::BlockType BlockType);

	/**
	* Retrieves the type of block in the chunk at a specific position.
	*/
	FBlock::BlockType GetBlock(const Vector3i& Position) const;

	/**
	* Destroys a block in the chunk at a specific position.
	*/
	void DestroyBlock(const Vector3i& Position);

	/**
	* Checks if this chunk contains any blocks.
	*/
	bool IsEmpty() const { return mIsEmpty; }

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
	struct MaskInfo
	{
		Vector4i AOFactors;
		FBlock Block;
	};

	void CheckAOSides(bool Sides[8], int32_t u, int32_t v, int32_t d, Vector3i BlockPosition, int32_t DepthOffset) const;

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
					const MaskInfo& FaceInfo,
					std::vector<FVoxelVertex>& VerticesOut,
					std::vector<uint32_t>& IndicesOut);

	/**
	* Computes ambient occlusion factors for each vertex of a voxel face.
	* @param Sides - The status of present blocks at surrounding positions.
	* ---------
	* | 0 1 2 | // Surrounding blocks are closer than C
	* | 3 C 4 | // C = Current
	* | 5 6 7 |
	* ---------
	* @return A 4 component vector with AO factors for each vertex. 
	*         x = top-left, y = bottom-left, z = bottom-right, w = top-right
	*/
	Vector4i ComputeBlockFaceAO(bool Sides[8]) const;

	/**
	* Computes the ambient occlusion factor of a voxel vertex.
	* ---------
	* | 0 1 2 | // Surrounding blocks are closer than C
	* | 3 C 4 | // C = Current
	* | 5 6 7 |
	* ---------
	* Params for calculating top-left vertex of C.
	* @param Side1 - If a voxel is present at 3.
	* @param Side2 - If a voxel is present at 1.
	* @param Corner - If a voxel is present at 0.
	* @return A value from 0 - 3. 3 means the vertex is not occluded.
	*/
	int32_t ComputeBlockVertexAO(bool Side1, bool Side2, bool Corner) const;

	/**
	* Determines if two voxel faces can be meshed together based on type and
	* ambient occlusion factors.
	*/
	bool IsMeshable(const MaskInfo& Lhs, const MaskInfo& Rhs) const;


private:
	FBlock* mBlocks;
	TMesh<FVoxelVertex>* mMesh;
	FChunkManager* mChunkManager;
	bool mIsLoaded;
	bool mIsEmpty;
};