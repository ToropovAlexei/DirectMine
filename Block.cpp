#include "pch.h"
#include "Block.h"

Block::Block(std::string name, std::vector<std::string> textures) noexcept :
	m_name(name)
{
	BuildTextures(textures);
}

std::string& Block::GetName()
{
	return m_name;
}

std::string& Block::GetFaceTexture(BlockFaces face)
{
	return m_textures[static_cast<size_t>(face)];
}

void Block::BuildTextures(std::vector<std::string> textures) noexcept
{
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
		m_textures[0] = textures[1];
		// Back
		m_textures[1] = textures[1];
		// Top
		m_textures[2] = textures[0];
		// Bottom
		m_textures[3] = textures[2];
		// Left
		m_textures[4] = textures[1];
		// Right
		m_textures[5] = textures[1];
		return;
	}
	throw std::runtime_error("Failed to build textures for block " + m_name);
}