#pragma once

#include "FastNoiseLite.h"
#include "Chunk.h"
#include "FastNoise/FastNoise.h"

class WorldGenerator
{
public:
	WorldGenerator(BlockManager& m_blockManager);

	std::unique_ptr<Chunk> GenerateChunk(int cx, int cz);

private:
	FastNoise::SmartNode<FastNoise::OpenSimplex2> heightGenNoise;
	FastNoise::SmartNode<FastNoise::FractalFBm> heightGenFBm;
	FastNoise::SmartNode<FastNoise::OpenSimplex2> tempNoise;
	FastNoise::SmartNode<> node;
	static const int maxHeight = 128;
	static const int minHeight = 45;
	static const int waterLevel = 62;
	BlockManager& m_blockManager;
};

