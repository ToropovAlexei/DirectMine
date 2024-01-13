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
	FastNoiseLite caveNoiseGen;
	static const int maxHeight = 64;
	static const int minHeight = 20;
	BlockManager& m_blockManager;
};

