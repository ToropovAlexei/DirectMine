#pragma once

#include "Block.h"
#include "BlockId.h"

class BlockManager
{
public:
	BlockManager() = default;

	void LoadBlocks();

	Block& GetBlockById(BlockId id);
	Block& GetBlockByName(std::string& name);
	BlockId GetBlockIdByName(std::string& name);

private:
	void BuildBlocksMap(std::vector<Block>& blocks);

private:
	std::unordered_map<BlockId, Block> m_blocksById;
	std::unordered_map<std::string, Block> m_blocksByName;
};

