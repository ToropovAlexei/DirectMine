#include "pch.h"
#include "WorldGenerator.h"

WorldGenerator::WorldGenerator(BlockManager& blockManager) :
    m_blockManager(blockManager)
{
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.1f);
    noise.SetFractalOctaves(3);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    caveNoiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    caveNoiseGen.SetFrequency(0.1f);
    caveNoiseGen.SetFractalOctaves(2);
}

Chunk WorldGenerator::GenerateChunk(ChunkPos& chunkPos)
{
    Chunk chunk = Chunk(chunkPos);

    for (int x = 0; x < Chunk::WIDTH; ++x) {
        for (int z = 0; z < Chunk::WIDTH; ++z) {
            float heightNoise = noise.GetNoise(static_cast<float>(chunkPos.x * Chunk::WIDTH + x) / Chunk::WIDTH, static_cast<float>(chunkPos.z * Chunk::WIDTH + z) / Chunk::WIDTH);
            float caveNoise = caveNoiseGen.GetNoise(static_cast<float>(chunkPos.x * Chunk::WIDTH + x) / Chunk::WIDTH, static_cast<float>(chunkPos.z * Chunk::WIDTH + z) / Chunk::WIDTH);

            int height = static_cast<int>((heightNoise + 1.0f) * (maxHeight - minHeight) / 2.0f) + minHeight;

            for (int y = 0; y < height; ++y) {
                if (y == 0)
                {
                    chunk.SetBlock(x, y, z, BlockId::Bedrock);
                }
                else if (y == height - 1)
                {
                    chunk.SetBlock(x, y, z, BlockId::Grass);
                }
                else if (y > height - 4) {
                    chunk.SetBlock(x, y, z, BlockId::Dirt);
                }
                else {
                    float oreNoise = noise.GetNoise(static_cast<float>(chunkPos.x * Chunk::WIDTH + x), static_cast<float>(chunkPos.z * Chunk::WIDTH + z), 2.0f);

                    if (caveNoise > 0.7f && y < height - 10) { // Проверка наличия пещеры
                        continue;
                    }
                    else if (oreNoise > 0.6f) {
                        chunk.SetBlock(x, y, z, BlockId::Stone);
                    }
                    else if (oreNoise > 0.3f) {
                        chunk.SetBlock(x, y, z, BlockId::IronOre);
                    }
                    else if (oreNoise > 0.1f) {
                        chunk.SetBlock(x, y, z, BlockId::GoldOre);
                    }
                    else if (oreNoise > 0.05f) {
                        chunk.SetBlock(x, y, z, BlockId::DiamondOre);
                    }
                    else
                    {
                        chunk.SetBlock(x, y, z, BlockId::Stone);
                    }
                }
            }
        }
    }

    return chunk;
}
