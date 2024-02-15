#include "pch.h"
#include "WorldGenerator.h"
#include "FastNoise/FastNoise.h"

WorldGenerator::WorldGenerator(BlockManager& blockManager) :
    m_blockManager(blockManager)
{
    fnSimplex = FastNoise::New<FastNoise::OpenSimplex2>();
    tempNoise = FastNoise::New<FastNoise::OpenSimplex2>();
}

Chunk WorldGenerator::GenerateChunk(int cx, int cz)
{
    Chunk chunk = Chunk(cx, cz);
    //for (int x = 0; x < Chunk::WIDTH; x++)
    //{
    //    for (int z = 0; z < Chunk::WIDTH; z++)
    //    {
    //        for (int y = 0; y < 50; y++)
    //        {
    //            chunk.SetBlock(x, y, z, BlockId::Water);
    //        }
    //    }
    //}
    //for (int x = 5; x < 6; x++)
    //{
    //    for (int z = 5; z < 6; z++)
    //    {
    //        for (int y = 49; y < 50; y++)
    //        {
    //            chunk.SetBlock(x, y, z, BlockId::Debug);
    //        }
    //    }
    //}
    //for (int y = 49; y < 50; y++)
    //{
    //    chunk.SetBlock(4, y, 4, BlockId::Leaves);
    //    chunk.SetBlock(4, y, 5, BlockId::Leaves);
    //    chunk.SetBlock(4, y, 6, BlockId::Leaves);
    //    chunk.SetBlock(5, y, 6, BlockId::Leaves);
    //    chunk.SetBlock(5, y, 4, BlockId::Leaves);
    //    chunk.SetBlock(6, y, 4, BlockId::Leaves);
    //    chunk.SetBlock(6, y, 5, BlockId::Leaves);
    //    chunk.SetBlock(6, y, 6, BlockId::Leaves);
    //}
    std::vector<float> heights(Chunk::SQ_WIDTH);
    std::vector<float> temps(Chunk::SQ_WIDTH);
    fnSimplex->GenUniformGrid2D(heights.data(), cx * Chunk::WIDTH, cz * Chunk::WIDTH, Chunk::WIDTH, Chunk::WIDTH, 0.005f, 1337);
    tempNoise->GenUniformGrid2D(temps.data(), cx * Chunk::WIDTH, cz * Chunk::WIDTH, Chunk::WIDTH, Chunk::WIDTH, 0.001f, 1337);

    size_t idx = 0;
    for (int z = 0; z < Chunk::WIDTH; ++z) {
        for (int x = 0; x < Chunk::WIDTH; ++x) {
            size_t activeIdx = idx++;
            //float caveNoise = caveNoiseGen.GetNoise(static_cast<float>(cx * Chunk::WIDTH + x) / Chunk::WIDTH, static_cast<float>(cz * Chunk::WIDTH + z) / Chunk::WIDTH);

            int height = static_cast<int>((heights[activeIdx] + 1.0f) * (maxHeight - minHeight) / 2.0f) + minHeight;

            for (int y = 0; y < std::max(height, waterLevel); ++y) {
                if (y > height)
                {
                    chunk.SetBlock(x, y, z, BlockId::Water);
                    continue;
                }
                chunk.SetBlock(x, y, z, temps[activeIdx] > 0.5 ? BlockId::Sand : BlockId::Grass);
                //if (y == 0)
                //{
                //    chunk.SetBlock(x, y, z, BlockId::Bedrock);
                //}
                //else if (y == height - 1)
                //{
                //    chunk.SetBlock(x, y, z, BlockId::Grass);
                //}
                //else if (y > height - 4) {
                //    chunk.SetBlock(x, y, z, BlockId::Dirt);
                //}
                //else {
                //    chunk.SetBlock(x, y, z, BlockId::Stone);
                //    //float oreNoise = noise.GetNoise(static_cast<float>(cx * Chunk::WIDTH + x), static_cast<float>(cz * Chunk::WIDTH + z), 2.0f);

                //    //if (caveNoise > 0.7f && y < height - 10) { // Проверка наличия пещеры
                //    //    continue;
                //    //}
                //    //else if (oreNoise > 0.6f) {
                //    //    chunk.SetBlock(x, y, z, BlockId::Stone);
                //    //}
                //    //else if (oreNoise > 0.3f) {
                //    //    chunk.SetBlock(x, y, z, BlockId::IronOre);
                //    //}
                //    //else if (oreNoise > 0.1f) {
                //    //    chunk.SetBlock(x, y, z, BlockId::GoldOre);
                //    //}
                //    //else if (oreNoise > 0.05f) {
                //    //    chunk.SetBlock(x, y, z, BlockId::DiamondOre);
                //    //}
                //    //else
                //    //{
                //    //    chunk.SetBlock(x, y, z, BlockId::Stone);
                //    //}
                //}
            }
        }
    }

    return chunk;
}
