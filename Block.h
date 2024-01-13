#pragma once

#include "BlockId.h"

class Block
{
public:
	enum class BlockFaces
	{
		Front,
		Back,
		Top,
		Bottom,
		Left,
		Right
	};
public:
	Block();
	Block(BlockId id, std::string name, std::vector<std::string> textures);

	std::string& GetName();
	std::string& GetFaceTexture(BlockFaces face);
	BlockId GetId() const noexcept;

private:
	void BuildTextures(std::vector<std::string>);

private:
	std::string m_name;
	BlockId m_id;
	std::array<std::string, 6u> m_textures; // Front, Back, Top, Bottom, Left, Right
};

