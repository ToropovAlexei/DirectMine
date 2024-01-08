#include "pch.h"
#include "ChunkBlock.h"

ChunkBlock::ChunkBlock(uint32_t id) noexcept :
	m_id(id), m_dir(ChunkBlock::BlockDirection::North)
{
}

ChunkBlock::ChunkBlock(uint32_t id, BlockDirection dir) noexcept :
	m_id(id), m_dir(dir)
{
}

uint32_t ChunkBlock::GetId() const noexcept
{
	return m_id;
}

ChunkBlock::BlockDirection ChunkBlock::GetDirection() const noexcept
{
	return m_dir;
}
