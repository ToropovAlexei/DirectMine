#pragma once

#include "Block.h"
#include <filesystem>

class BlockLoader
{
public:
	BlockLoader() = default;

	void LoadBlocks();

	std::vector<Block>& GetBlocks();

private:
	void LoadBlock(std::filesystem::path filePath);

private:
	std::vector<Block> m_blocks;
};

