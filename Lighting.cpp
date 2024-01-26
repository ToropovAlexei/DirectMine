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
