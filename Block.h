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
	Block(BlockId id, std::string name, std::vector<std::string> textures, std::array<uint8_t, 3u> emission);

	std::string& GetName();
	std::string& GetFaceTexture(BlockFaces face);
	BlockId GetId() const noexcept;
	bool IsOpaque() const noexcept;
	const std::array<uint8_t, 3u>& GetEmission() const noexcept;

private:
	void BuildTextures(std::vector<std::string>);

private:
	std::string m_name;
	bool m_isOpaque;
	BlockId m_id;
	std::array<std::string, 6u> m_textures; // Front, Back, Top, Bottom, Left, Right
	std::array<uint8_t, 3u> m_emission; // RGB
};

