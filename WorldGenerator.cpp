#include "pch.h"
#include "WorldGenerator.h"

WorldGenerator::WorldGenerator(BlockManager& blockManager) :
    m_blockManager(blockManager)
{
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.4f);
}

std::unique_ptr<Chunk> WorldGenerator::GenerateChunk(WorldPos& worldPos)
{
    std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(worldPos);
    for (int x = 0; x < Chunk::WIDTH; ++x) {
        for (int z = 0; z < Chunk::DEPTH; ++z) {
            float noiseValue = noise.GetNoise(static_cast<float>(worldPos.x + x) / Chunk::WIDTH, static_cast<float>(worldPos.z + z) / Chunk::DEPTH);

            int height = static_cast<int>((noiseValue + 1.0f) * Chunk::HEIGHT / 2.0f);

            for (int y = static_cast<int>(worldPos.y); y < height; ++y) {
                WorldPos pos = WorldPos(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
                if (y == 0)
                {
                    chunk->AddBlock(pos, BlockId::Bedrock);
                }
                else if (y == height - 1)
                {
                    chunk->AddBlock(pos, BlockId::Grass);
                }
                else if (y == height - 2) {
                    chunk->AddBlock(pos, BlockId::Dirt);
                }
                else {
                    float oreNoise = noise.GetNoise(static_cast<float>(x) / Chunk::WIDTH, static_cast<float>(z) / Chunk::DEPTH, 2.0f);

                    if (oreNoise > 0.6f) {
                        chunk->AddBlock(pos, BlockId::Stone);
                    }
                    else if (oreNoise > 0.3f) {
                        chunk->AddBlock(pos, BlockId::IronOre);
                    }
                    else if (oreNoise > 0.1f) {
                        chunk->AddBlock(pos, BlockId::GoldOre);
                    }
                    else if (oreNoise > 0.05f) {
                        chunk->AddBlock(pos, BlockId::DiamondOre);
                    }
                    else
                    {
                        chunk->AddBlock(pos, BlockId::Stone);
                    }
                }
            }
        }
    }

	return chunk;
}
