#pragma once

#include "WorldPos.hpp"
#include "BlockManager.h"
#include "LightSolver.h"
#include "Chunk.h"

class ChunksManager;

class Lighting
{
public:
	Lighting(ChunksManager* chunksManager, BlockManager& blockManager);

	void HandleBlockSet(int x, int y, int z, std::shared_ptr<Chunk> chunk, ChunkBlock block);
	void BuildSunlight(std::shared_ptr<Chunk> chunk);
	void Solve();

public:
	std::unique_ptr<LightSolver> solverS;
	std::unique_ptr<LightSolver> solverR;
	std::unique_ptr<LightSolver> solverG;
	std::unique_ptr<LightSolver> solverB;

private:
	inline void AddToSolvers(int x, int y, int z, std::shared_ptr<Chunk> chunk);

private:
	ChunksManager* m_chunksManager;
	BlockManager& m_blockManager;
};
