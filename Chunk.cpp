#include "pch.h"
#include "Chunk.h"
#include "GrassBlock.h"

Chunk::Chunk()
{
	for (size_t i = 0; i < m_blocks.size(); i++)
	{
		m_blocks[i] = std::make_unique<GrassBlock>(DirectX::XMFLOAT3());
	}
}

const std::array<std::unique_ptr<Cube>, Chunk::VOLUME>& Chunk::GetBlocks() const noexcept
{
	return m_blocks;
}
