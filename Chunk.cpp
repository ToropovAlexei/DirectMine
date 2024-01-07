#include "pch.h"
#include "Chunk.h"

Chunk::Chunk()
{

}

const std::unordered_map<Chunk::BlockPos, Cube, Chunk::BlockPosHash>& Chunk::GetBlocks() const noexcept
{
	return m_blocks;
}

void Chunk::FillWith()
{
	for (size_t x = 0; x < Chunk::WIDTH; x++)
	{
		for (size_t y = 0; y < Chunk::HEIGHT; y++)
		{
			for (size_t z = 0; z < Chunk::DEPTH; z++)
			{
				BlockPos pos = BlockPos(x, y, z);
				m_blocks.insert({ pos, Cube() });
			}
		}
	}
}
