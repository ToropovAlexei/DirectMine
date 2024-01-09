#include "pch.h"
#include "ChunkBlock.h"

ChunkBlock::ChunkBlock(uint16_t id) noexcept :
	m_id(id), m_dir(ChunkBlock::BlockDirection::North)
{
}

ChunkBlock::ChunkBlock(uint16_t id, BlockDirection dir) noexcept :
	m_id(id), m_dir(dir)
{
}

uint16_t ChunkBlock::GetId() const noexcept
{
	return m_id;
}

ChunkBlock::BlockDirection ChunkBlock::GetDirection() const noexcept
{
	return m_dir;
}
