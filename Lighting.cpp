#include "pch.h"
#include "Lighting.h"
#include "ChunksManager.h"

Lighting::Lighting(ChunksManager* chunksManager, BlockManager& blockManager) :
	m_chunksManager(chunksManager),
	m_blockManager(blockManager)
{
	solverS = std::make_unique<LightSolver>(chunksManager, m_blockManager, 3);
	solverR = std::make_unique<LightSolver>(chunksManager, m_blockManager, 2);
	solverG = std::make_unique<LightSolver>(chunksManager, m_blockManager, 1);
	solverB = std::make_unique<LightSolver>(chunksManager, m_blockManager, 0);
}

void Lighting::HandleBlockSet(int x, int y, int z, std::shared_ptr<Chunk> chunk, ChunkBlock block)
{
	auto& blockType = m_blockManager.GetBlockById(block.GetId());
	auto& emission = blockType.GetEmission();
	solverR->Remove({ x, y, z, chunk->GetLightmapRef().GetR(x, y, z), chunk });
	solverG->Remove({ x, y, z, chunk->GetLightmapRef().GetG(x, y, z), chunk });
	solverB->Remove({ x, y, z, chunk->GetLightmapRef().GetB(x, y, z), chunk });
	chunk->GetLightmapRef().SetR(x, y, z, emission[0]);
	chunk->GetLightmapRef().SetG(x, y, z, emission[1]);
	chunk->GetLightmapRef().SetB(x, y, z, emission[2]);
	solverR->Solve();
	solverG->Solve();
	solverB->Solve();
	if (block.GetId() == BlockId::Air)
	{
		solverR->Add({ x, y + 1, z, chunk }); solverG->Add({ x, y + 1, z, chunk }); solverB->Add({ x, y + 1, z, chunk }); solverS->Add({ x, y + 1, z, chunk });
		solverR->Add({ x, y - 1, z, chunk }); solverG->Add({ x, y - 1, z, chunk }); solverB->Add({ x, y - 1, z, chunk }); solverS->Add({ x, y - 1, z, chunk });
		solverR->Add({ x + 1, y, z, chunk }); solverG->Add({ x + 1, y, z, chunk }); solverB->Add({ x + 1, y, z, chunk }); solverS->Add({ x + 1, y, z, chunk });
		solverR->Add({ x - 1, y, z, chunk }); solverG->Add({ x - 1, y, z, chunk }); solverB->Add({ x - 1, y, z, chunk }); solverS->Add({ x - 1, y, z, chunk });
		solverR->Add({ x, y, z + 1, chunk }); solverG->Add({ x, y, z + 1, chunk }); solverB->Add({ x, y, z + 1, chunk }); solverS->Add({ x, y, z + 1, chunk });
		solverR->Add({ x, y, z - 1, chunk }); solverG->Add({ x, y, z - 1, chunk }); solverB->Add({ x, y, z - 1, chunk }); solverS->Add({ x, y, z - 1, chunk });
		solverR->Solve();
		solverG->Solve();
		solverB->Solve();
	}
	else
	{
		if (emission[0] || emission[1] || emission[2]) {
			solverR->Add({ x, y, z, chunk });
			solverG->Add({ x, y, z, chunk });
			solverB->Add({ x, y, z, chunk });
			solverR->Solve();
			solverG->Solve();
			solverB->Solve();
		}
	}
}

void Lighting::BuildSunlight(std::shared_ptr<Chunk> chunk)
{
	for (int x = 0; x < Chunk::WIDTH; x++)
	{
		for (int z = 0; z < Chunk::WIDTH; z++)
		{
		}
	}
}
