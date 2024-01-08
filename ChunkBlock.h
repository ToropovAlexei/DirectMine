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
	ChunkBlock(uint32_t id) noexcept;
	ChunkBlock(uint32_t id, BlockDirection dir) noexcept;

	uint32_t GetId() const noexcept;
	BlockDirection GetDirection() const noexcept;

private:
	uint32_t m_id;
	BlockDirection m_dir;
};

