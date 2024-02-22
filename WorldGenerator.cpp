#include "pch.h"
#include "WorldGenerator.h"
#include "FastNoise/FastNoise.h"

WorldGenerator::WorldGenerator(BlockManager& blockManager) :
    m_blockManager(blockManager)
{
    heightGenNoise = FastNoise::New<FastNoise::OpenSimplex2>();
    heightGenFBm = FastNoise::New<FastNoise::FractalFBm>();
    heightGenFBm->SetSource(heightGenNoise);
    heightGenFBm->SetOctaveCount(2);
    tempNoise = FastNoise::New<FastNoise::OpenSimplex2>();
    node = FastNoise::NewFromEncodedNodeTree("EQACAAAAAAAgQBAAAAAAQBkAEwDD9Sg/DQAEAAAAAAAgQAkAAGZmJj8AAAAAPwEEAAAAAAAAAEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAM3MTD4AMzMzPwAAAAA/");
}

Chunk WorldGenerator::GenerateChunk(int cx, int cz)
{
    Chunk chunk = Chunk(cx, cz);
    std::vector<float> heights(Chunk::SQ_WIDTH);
    std::vector<float> temps(Chunk::SQ_WIDTH);
    heightGenFBm->GenUniformGrid2D(heights.data(), cx * Chunk::WIDTH, cz * Chunk::WIDTH, Chunk::WIDTH, Chunk::WIDTH, 0.005f, 1337);
    tempNoise->GenUniformGrid2D(temps.data(), cx * Chunk::WIDTH, cz * Chunk::WIDTH, Chunk::WIDTH, Chunk::WIDTH, 0.002f, 1337);

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
