#pragma once

#include "BlockId.h"

class ChunkBlock
{
public:
	enum class BlockDirection
	{
		North,
		West,
		South,
		East,
		Up,
		Down,
	};
public:
	ChunkBlock(BlockId id) noexcept;
	ChunkBlock(BlockId id, BlockDirection dir) noexcept;

	BlockId GetId() const noexcept;
	BlockDirection GetDirection() const noexcept;

private:
	BlockId m_id;
	BlockDirection m_dir;
};

