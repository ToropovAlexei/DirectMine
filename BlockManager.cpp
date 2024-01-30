#include "pch.h"
#include "BlockManager.h"
#include "BlockLoader.h"

void BlockManager::LoadBlocks()
{
	BlockLoader blockLoader = BlockLoader();
	blockLoader.LoadBlocks();
	std::vector<Block> blocks = blockLoader.GetBlocks();
	BuildBlocksMap(blocks);
}

Block& BlockManager::GetBlockById(BlockId id)
{
	if (!m_blocksById.contains(id))
	{
		throw std::runtime_error("Block with id " + std::to_string(static_cast<uint16_t>(id)) + " not found");
	}
	return m_blocksById[id];
}

void BlockManager::BuildBlocksMap(std::vector<Block>& blocks)
{
	for (size_t i = 0; i < blocks.size(); i++)
	{
		m_blocksById.insert({ blocks[i].GetId(), blocks[i]});
	}
}
