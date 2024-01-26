#include "pch.h"
#include "Lighting.h"

Lighting::Lighting(std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks, BlockManager& blockManager) :
	m_chunks(chunks),
	m_blockManager(blockManager)
{
	solverS = std::make_unique<LightSolver>(m_chunks, m_blockManager, 3);
	solverR = std::make_unique<LightSolver>(m_chunks, m_blockManager, 2);
	solverG = std::make_unique<LightSolver>(m_chunks, m_blockManager, 1);
	solverB = std::make_unique<LightSolver>(m_chunks, m_blockManager, 0);
}

void Lighting::HandleBlockSet(int x, int y, int z, std::shared_ptr<Chunk> chunk, ChunkBlock block)
{
	auto& emission = m_blockManager.GetBlockById(block.GetId()).GetEmission();
	if (emission[0] || emission[1] || emission[2])
	{
		chunk->GetLightmapRef().SetR(x, y, z, std::max(chunk->GetLightmapRef().GetR(x, y, z), static_cast<int>(emission[0])));
		chunk->GetLightmapRef().SetG(x, y, z, std::max(chunk->GetLightmapRef().GetG(x, y, z), static_cast<int>(emission[1])));
		chunk->GetLightmapRef().SetB(x, y, z, std::max(chunk->GetLightmapRef().GetB(x, y, z), static_cast<int>(emission[2])));
		solverR->Add({ x, y, z, chunk });
		solverG->Add({ x, y, z, chunk });
		solverB->Add({ x, y, z, chunk });
		solverS->Add({ x, y, z, chunk });
	}
}
