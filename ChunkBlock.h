#pragma once

#include "BlockId.h"

class ChunkBlock
{
public:
	enum class BlockDirection : uint8_t
	{
		North,
		West,
		South,
		East,
		Up,
		Down,
	};
public:
	ChunkBlock();
	ChunkBlock(BlockId id) noexcept;
	ChunkBlock(BlockId id, BlockDirection dir) noexcept;

	inline BlockId GetId() const noexcept { return m_id; };
	inline BlockDirection GetDirection() const noexcept { return m_dir; };

private:
	BlockId m_id;
	BlockDirection m_dir;
};

