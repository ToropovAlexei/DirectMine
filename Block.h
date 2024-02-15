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
	Block(BlockId id, std::string name, std::vector<std::string> textures, std::array<uint8_t, 3u> emission, bool isOpaque);
	Block(BlockId id, std::string name, std::vector<std::string> textures, std::array<uint8_t, 3u> emission, bool isOpaque, int drawGroup);

	inline std::string& GetName() noexcept
	{
		return m_name;
	};
	inline std::string& GetFaceTexture(BlockFaces face) noexcept
	{
		return m_textures[static_cast<size_t>(face)];
	};
	inline BlockId GetId() const noexcept
	{
		return m_id;
	};
	inline bool IsOpaque() const noexcept
	{
		return m_isOpaque;
	};
	inline const std::array<uint8_t, 3u>& GetEmission() const noexcept
	{
		return m_emission;
	};
	inline int GetDrawGroup() const noexcept
	{
		return m_drawGroup;
	}

private:
	void BuildTextures(std::vector<std::string>);

private:
	std::string m_name;
	bool m_isOpaque;
	BlockId m_id;
	int m_drawGroup;
	std::array<std::string, 6u> m_textures; // Front, Back, Top, Bottom, Left, Right
	std::array<uint8_t, 3u> m_emission; // RGB
};

