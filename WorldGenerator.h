#pragma once

#include "FastNoiseLite.h"
#include "Chunk.h"
#include "FastNoise/FastNoise.h"

class WorldGenerator
{
public:
	WorldGenerator(BlockManager& m_blockManager);

	Chunk GenerateChunk(int cx, int cz);

private:
	FastNoise::SmartNode<FastNoise::OpenSimplex2> fnSimplex;
	static const int maxHeight = 64;
	static const int minHeight = 20;
	BlockManager& m_blockManager;
};

