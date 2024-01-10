#include "pch.h"
#include "BlockLoader.h"
#include <nlohmann/json.hpp>
#include <fstream>

void BlockLoader::LoadBlocks()
{
	std::string path = "blocks";
	for (const auto& blockPath : std::filesystem::directory_iterator(path))
	{
		LoadBlock(blockPath.path());
	}
	if (m_blocks.empty())
	{
		throw std::runtime_error("Blocks not found in " + path + " path");
	}
}

std::vector<Block>& BlockLoader::GetBlocks()
{
	return m_blocks;
}

void BlockLoader::LoadBlock(std::filesystem::path filePath)
{
	std::ifstream f(filePath);
	nlohmann::json blockData = nlohmann::json::parse(f);
	std::vector<std::string> textures = blockData["textures"];
	std::string name = blockData["name"];
	BlockId id = blockData["id"];
	m_blocks.push_back(Block(id, name, textures));
}
