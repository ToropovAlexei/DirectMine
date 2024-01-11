#pragma once

#include "FastNoiseLite.h"
#include "Chunk.h"

class WorldGenerator
{
public:
	WorldGenerator(BlockManager& m_blockManager);

	std::unique_ptr<Chunk> GenerateChunk(ChunkPos& chunkPos);

private:
	FastNoiseLite noise;
	static const int maxHeight = 64;
	BlockManager& m_blockManager;
};

