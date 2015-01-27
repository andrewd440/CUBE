#pragma once

#include <vector>
#include <queue>
#include "Chunk.h"

class FChunkManager
{
public:
	static const uint32_t WORLD_WIDTH = 512;
	static const uint32_t WORLD_HEIGHT = 1;
public:
	FChunkManager();
	~FChunkManager();

	void Setup();

	void Update();

	void Render();

private:
	void UpdateUnloadList();
	void UpdateLoadList();
	void UpdateRebuildList();
	void UpdateVisibleList();
	void UpdateRenderList();

private:
	std::vector<FChunk> mChunks;        // All world chunks
	std::vector<uint32_t> mVisibleList; // Index list of potentially visible chunks
	std::vector<uint32_t> mRenderList;  // Index list of chunks to render
	std::queue<uint32_t> mLoadList;     // Index list of chunks to be loaded
	std::queue<uint32_t> mUnloadList;   // Index list of chunks to be unloaded
	std::queue<uint32_t> mRebuildList;  // Index list of chunks to be rebuilt
};

