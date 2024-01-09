#pragma once
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
	ChunkBlock(uint16_t id) noexcept;
	ChunkBlock(uint16_t id, BlockDirection dir) noexcept;

	uint16_t GetId() const noexcept;
	BlockDirection GetDirection() const noexcept;

private:
	uint16_t m_id;
	BlockDirection m_dir;
};

