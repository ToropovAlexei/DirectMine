#pragma once

#include "WorldPos.hpp"
#include "BlockManager.h"
#include "LightSolver.h"
#include "Chunk.h"

class Lighting
{
public:
	Lighting(std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks, BlockManager& blockManager);

	void HandleBlockSet(int x, int y, int z, std::shared_ptr<Chunk> chunk, ChunkBlock block);
	void BuildSunlight(std::shared_ptr<Chunk> chunk);

public:
	std::unique_ptr<LightSolver> solverS;
	std::unique_ptr<LightSolver> solverR;
	std::unique_ptr<LightSolver> solverG;
	std::unique_ptr<LightSolver> solverB;

private:
	std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& m_chunks;
	BlockManager& m_blockManager;
};
