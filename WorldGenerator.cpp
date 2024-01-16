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
    auto start = std::chrono::high_resolution_clock::now();
    Chunk chunk = Chunk(chunkPos);

    for (int x = 0; x < Chunk::WIDTH; ++x) {
        for (int z = 0; z < Chunk::DEPTH; ++z) {
            float heightNoise = noise.GetNoise(static_cast<float>(chunkPos.x + x) / Chunk::WIDTH, static_cast<float>(chunkPos.z + z) / Chunk::DEPTH);
            float caveNoise = caveNoiseGen.GetNoise(static_cast<float>(chunkPos.x + x) / Chunk::WIDTH, static_cast<float>(chunkPos.z + z) / Chunk::DEPTH);

            int height = static_cast<int>((heightNoise + 1.0f) * (maxHeight - minHeight) / 2.0f) + minHeight;

            for (int y = 0; y < height; ++y) {
                WorldPos pos = WorldPos(x, y, z);
                if (y == 0)
                {
                    chunk.AddBlock(pos, BlockId::Bedrock);
                }
                else if (y == height - 1)
                {
                    chunk.AddBlock(pos, BlockId::Grass);
                }
                else if (y > height - 4) {
                    chunk.AddBlock(pos, BlockId::Dirt);
                }
                else {
                    float oreNoise = noise.GetNoise(static_cast<float>(chunkPos.x + x), static_cast<float>(chunkPos.z + z), 2.0f);

                    if (caveNoise > 0.7f && y < height - 10) { // Проверка наличия пещеры
                        continue;
                    }
                    else if (oreNoise > 0.6f) {
                        chunk.AddBlock(pos, BlockId::Stone);
                    }
                    else if (oreNoise > 0.3f) {
                        chunk.AddBlock(pos, BlockId::IronOre);
                    }
                    else if (oreNoise > 0.1f) {
                        chunk.AddBlock(pos, BlockId::GoldOre);
                    }
                    else if (oreNoise > 0.05f) {
                        chunk.AddBlock(pos, BlockId::DiamondOre);
                    }
                    else
                    {
                        chunk.AddBlock(pos, BlockId::Stone);
                    }
                }
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::string result = "Время выполнения GenerateChunk: " + std::to_string(duration.count()) + " микросекунд\n";
    OutputDebugStringA(result.c_str());

    return chunk;
}
