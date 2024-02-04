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

void BlockManager::BuildBlocksMap(std::vector<Block>& blocks)
{
	m_blocks.resize(static_cast<size_t>(BlockId::Count));
	for (size_t i = 0; i < blocks.size(); i++)
	{
		m_blocks[static_cast<size_t>(blocks[i].GetId())] = blocks[i];
		m_blocksById.insert({ blocks[i].GetId(), blocks[i]});
	}
}
