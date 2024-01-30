#include "pch.h"
#include "ChunkBlock.h"

ChunkBlock::ChunkBlock() :
	m_id(BlockId::Air), m_dir(ChunkBlock::BlockDirection::North)
{
}

ChunkBlock::ChunkBlock(BlockId id) noexcept :
	m_id(id), m_dir(ChunkBlock::BlockDirection::North)
{
}

ChunkBlock::ChunkBlock(BlockId id, BlockDirection dir) noexcept :
	m_id(id), m_dir(dir)
{
}
