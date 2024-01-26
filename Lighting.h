#pragma once

#include "WorldPos.hpp"
#include "BlockManager.h"
#include "LightSolver.h"

class Chunk;

class Lighting
{
public:
	Lighting(std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks, BlockManager& blockManager);

public:
	std::unique_ptr<LightSolver> solverS;
	std::unique_ptr<LightSolver> solverR;
	std::unique_ptr<LightSolver> solverG;
	std::unique_ptr<LightSolver> solverB;

private:
	std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& m_chunks;
	BlockManager& m_blockManager;
};
