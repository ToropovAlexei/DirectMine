#pragma once

#include "FastNoiseLite.h"
#include "Chunk.h"

class WorldGenerator
{
public:
	WorldGenerator(BlockManager& m_blockManager);

	Chunk GenerateChunk(int cx, int cz);

private:
	FastNoiseLite noise;
	FastNoiseLite caveNoiseGen;
	static const int maxHeight = 64;
	static const int minHeight = 20;
	BlockManager& m_blockManager;
};

