#include "pch.h"
#include "Block.h"

Block::Block() :
	m_id(BlockId::Air), m_isOpaque(false), m_emission({})
{
}

Block::Block(BlockId id, std::string name, std::vector<std::string> textures) :
	m_id(id),
	m_name(name),
	m_emission({})
{
	BuildTextures(textures);
}

Block::Block(BlockId id, std::string name, std::vector<std::string> textures, std::array<uint8_t, 3u> emission) :
	m_id(id),
	m_name(name),
	m_emission(emission)
{
	BuildTextures(textures);
}

Block::Block(BlockId id, std::string name, std::vector<std::string> textures, std::array<uint8_t, 3u> emission, bool isOpaque) :
	m_id(id),
	m_name(name),
	m_isOpaque(isOpaque),
	m_emission(emission)
{
	BuildTextures(textures);
}

Block::Block(BlockId id, std::string name, std::vector<std::string> textures, std::array<uint8_t, 3u> emission, bool isOpaque, int drawGroup) :
	m_id(id),
	m_name(name),
	m_isOpaque(isOpaque),
	m_drawGroup(drawGroup),
	m_emission(emission)
{
	BuildTextures(textures);
}

void Block::BuildTextures(std::vector<std::string> textures)
{
	if (textures.empty())
	{
		return;
	}
	if (textures.size() == 1)
	{
		for (size_t i = 0; i < m_textures.size(); i++)
		{
			m_textures[i] = textures[0];
		}
		return;
	}
	if (textures.size() == 3)
	{
		// Front
		m_textures[static_cast<size_t>(BlockFaces::Front)] = textures[1];
		// Back
		m_textures[static_cast<size_t>(BlockFaces::Back)] = textures[1];
		// Top
		m_textures[static_cast<size_t>(BlockFaces::Top)] = textures[0];
		// Bottom
		m_textures[static_cast<size_t>(BlockFaces::Bottom)] = textures[2];
		// Left
		m_textures[static_cast<size_t>(BlockFaces::Left)] = textures[1];
		// Right
		m_textures[static_cast<size_t>(BlockFaces::Right)] = textures[1];
		return;
	}
	if (textures.size() == 6)
	{
		// Front
		m_textures[static_cast<size_t>(BlockFaces::Front)] = textures[0];
		// Back
		m_textures[static_cast<size_t>(BlockFaces::Back)] = textures[1];
		// Top
		m_textures[static_cast<size_t>(BlockFaces::Top)] = textures[2];
		// Bottom
		m_textures[static_cast<size_t>(BlockFaces::Bottom)] = textures[3];
		// Left
		m_textures[static_cast<size_t>(BlockFaces::Left)] = textures[4];
		// Right
		m_textures[static_cast<size_t>(BlockFaces::Right)] = textures[5];
		return;
	}
	throw std::runtime_error("Failed to build textures for block " + m_name);
}
