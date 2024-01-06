#include "pch.h"
#include "Chunk.h"
#include "GrassBlock.h"

Chunk::Chunk()
{
	for (size_t x = 0; x < Chunk::WIDTH; x++)
	{
		for (size_t y = 0; y < Chunk::HEIGHT; y++)
		{
			for (size_t z = 0; z < Chunk::DEPTH; z++)
			{
				m_blocks[x * Chunk::WIDTH + y + z * (Chunk::WIDTH * Chunk::HEIGHT)] = std::make_unique<GrassBlock>(DirectX::XMFLOAT3(x, y, z));
			}
		}
	}
}

const std::array<std::unique_ptr<Cube>, Chunk::VOLUME>& Chunk::GetBlocks() const noexcept
{
	return m_blocks;
}
