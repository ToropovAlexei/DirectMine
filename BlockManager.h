#pragma once

#include "Block.h"

class BlockManager
{
public:
	BlockManager() = default;

	void LoadBlocks();

	Block& GetBlockById(uint32_t id);
	Block& GetBlockByName(std::string name);

private:
	void BuildBlocksMap(std::vector<Block>& blocks);

private:
	std::unordered_map<uint32_t, Block> m_blocksById;
	std::unordered_map<std::string, Block> m_blocksByName;
};

