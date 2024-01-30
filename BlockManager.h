#pragma once

#include "Block.h"
#include "BlockId.h"

class BlockManager
{
public:
	BlockManager() = default;

	void LoadBlocks();

	inline Block& GetBlockById(BlockId id) noexcept { return m_blocksById[id]; };

private:
	void BuildBlocksMap(std::vector<Block>& blocks);

private:
	std::unordered_map<BlockId, Block> m_blocksById;
	std::unordered_map<std::string, Block> m_blocksByName;
};

