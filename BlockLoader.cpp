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
	std::vector<std::string> textures;
	if (blockData.contains("textures"))
	{
		textures = blockData["textures"];
	}
	std::string name = blockData["name"];
	BlockId id = blockData["id"];
	std::array<uint8_t, 3u> emission = {0u, 0u, 0u};
	if (blockData.contains("emission"))
	{
		std::vector<int> emissionVec = blockData["emission"];
		if (emissionVec.size() != 3)
		{
			throw std::runtime_error("Emission is incorrect in " + filePath.filename().string());
		}
		for (size_t i = 0; i < emission.size(); i++)
		{
			emission[i] = static_cast<uint8_t>(emissionVec[i]);
		}
	}
	bool isOpaque = true;
	if (blockData.contains("is_opaque"))
	{
		isOpaque = blockData["is_opaque"];
	}

	m_blocks.push_back(Block(id, name, textures, emission, isOpaque));
}
