#pragma once

#include "Cube.h"

class Chunk
{
public:
	struct BlockPos
	{
		float x;
		float y;
		float z;

		bool operator==(const BlockPos& other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}
	};

	struct BlockPosHash
	{
		std::size_t operator()(const BlockPos& pos) const
		{
			// Преобразование значений типа float в биты
			std::size_t xHash = std::hash<float>{}(pos.x);
			std::size_t yHash = std::hash<float>{}(pos.y);
			std::size_t zHash = std::hash<float>{}(pos.z);

			// Комбинирование хэшей с помощью побитового исключающего ИЛИ (XOR)
			std::size_t combinedHash = xHash ^ yHash ^ zHash;

			return combinedHash;
		}
	};
public:
	Chunk();

	static const int WIDTH = 32;
	static const int HEIGHT = 32;
	static const int DEPTH = 32;
	static constexpr int VOLUME = WIDTH * HEIGHT * HEIGHT;

	const std::unordered_map<BlockPos, Cube, BlockPosHash>& GetBlocks() const noexcept;

	void FillWith();

private:
	std::unordered_map<BlockPos, Cube, BlockPosHash> m_blocks;
};

