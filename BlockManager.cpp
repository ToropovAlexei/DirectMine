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

Block& BlockManager::GetBlockById(uint32_t id)
{
	if (!m_blocksById.contains(id))
	{
		throw std::runtime_error("Block with id " + std::to_string(id) + " not found");
	}
	return m_blocksById[id];
}

Block& BlockManager::GetBlockByName(std::string name)
{
	if (!m_blocksByName.contains(name))
	{
		throw std::runtime_error("Block with name " + name + " not found");
	}
	return m_blocksByName[name];
}

void BlockManager::BuildBlocksMap(std::vector<Block>& blocks)
{
	for (size_t i = 0; i < blocks.size(); i++)
	{
		m_blocksById.insert({ static_cast<uint32_t>(i), blocks[i] });
		m_blocksByName.insert({ blocks[i].GetName(), blocks[i]});
	}
}
