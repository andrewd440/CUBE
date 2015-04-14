#pragma once

#include <GL\glew.h>
#include <cstdint>
#include <atomic>

#include "Memory\PoolAllocator.h"
#include "Common.h"
#include "Block.h"
#include "BulletPhysics\btBulletCollisionCommon.h"
#include "Rendering\GLBindings.h"
#include "ChunkMesh.h"
#include "BlockTypes.h"

class FChunkManager;
class FPhysicsSystem;

/**
* Represents a 3D mesh of voxels of CHUNK_SIZE
* dimensions.
*/
class FChunk
{
private:
	WIN_ALIGN(16)
	struct CollisionData
	{
		struct MeshData
		{
			MeshData()
				: Mesh()
				, Shape(&Mesh, false, false)
			{}
			btTriangleIndexVertexArray Mesh;
			btBvhTriangleMeshShape     Shape;
		};

		CollisionData()
			: Object()
			, ActiveMesh(false)
		{}
		MeshData            Mesh[2]; // Double buffer for collision mesh data
		btCollisionObject   Object;
		bool                ActiveMesh;
	};

public:
	// Dimensions of each chunk
	static const int32_t CHUNK_SIZE = 32;
	static const int32_t BLOCKS_PER_CHUNK = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

	// Memory pools
	static const uint32_t POOL_SIZE = 30000;
	static FPoolAllocator<sizeof(FBlock) * BLOCKS_PER_CHUNK, POOL_SIZE> ChunkAllocator;
	static FPoolAllocatorType<FChunkMesh, POOL_SIZE> MeshAllocator;
	static FPoolAllocatorType<CollisionData, POOL_SIZE> CollisionAllocator;

public:
	/**
	* Returns the index of a block in the mBlocks array based on 3D coordinates within the chunk.
	*/
	static int32_t BlockIndex(Vector3i Position);
	static int32_t FChunk::BlockIndex(int32_t X, int32_t Y, int32_t Z);

public:
	/**
	* Constructs chunk of voxels.
	*/
	FChunk();

	FChunk(const FChunk& Other) = delete;
	FChunk& operator=(const FChunk& Other) = delete;

	/**
	* Frees data back to respective pools.
	*/
	~FChunk();

	/**
	* Allocates and builds chunk data. Chunk meshes will still need to 
	* be built before rendering.
	* @param BlockData - RLE block layout for this chunk.
	* @return True if the chunk is not empty, false otherwise.
	*/
	bool Load(const std::vector<uint8_t>& BlockData, const Vector3f& WorldPosition);

	/**
	* Frees block and mesh data.
	* @param BlockDataOut - Memory to place RLE block layout for this chunk.
	*/
	void Unload(std::vector<uint8_t>& BlockDataOut);

	/**
	* Removes data held by this chunk from external services.
	*/
	void ShutDown(FPhysicsSystem& PhysicsSystem);

	/**
	* Builds/Rebuilds this chunks' mesh.
	*/
	void RebuildMesh();

	/**
	* Swaps the currently used mesh for rendering.
	*/
	void SwapMeshBuffer(FPhysicsSystem& PhysicsSystem);

	/**
	* Checks if the chunk has been loaded.
	*/
	bool IsLoaded() const;

	/**
	* Renders active blocks in the chunk.
	*/
	void Render(const GLenum RenderMode = GL_TRIANGLES);

	/**
	* Set a block in the chunk at a specific position.
	*/
	void SetBlock(const Vector3i& Position, FBlockTypes::BlockID ID);

	/**
	* Retrieves the type of block in the chunk at a specific position.
	*/
	FBlockTypes::BlockID GetBlock(const Vector3i& Position) const;

	/**
	* Destroys a block in the chunk at a specific position.
	* @return ID of the block that was destroyed.
	*/
	FBlockTypes::BlockID DestroyBlock(const Vector3i& Position);

	/**
	* Checks if this chunk contains any blocks.
	*/
	bool IsEmpty() const { return mIsEmpty; }

private:
	// Constants used for constructing quads with correct normals in GreedyMesh()
	struct NormalID
	{
		enum : uint32_t
		{
			East,
			West,
			Top,
			Bottom,
			North,
			South
		};
	};

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
	* @param Type - The type of block the quad is used for.
	* @param VerticesOut - Location to place vertex data.
	* @param IndicesOut - Location to place index data.
	*/
	void AddQuad(	const Vector3ui& BottomLeft, 
					const Vector3ui& TopLeft, 
					const Vector3ui& TopRight, 
					const Vector3ui& BottomRight,
					const bool IsBackface, 
					const uint32_t Side, 
					const FBlock BlockType,
					std::vector<Vector3f>& VerticesOut,
					std::vector<FChunkMesh::RenderData>& RenderDataOut,
					std::vector<uint32_t>& IndicesOut);

private:
	FBlock* mBlocks;
	FChunkMesh* mMesh;
	CollisionData* mCollisionData;

	std::atomic_bool mIsLoaded;
	std::atomic_bool mIsEmpty;
};