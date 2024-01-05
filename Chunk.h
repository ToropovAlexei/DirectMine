#pragma once

#include "Cube.h"

class Chunk
{
public:
	Chunk();

	static const int WIDTH = 16;
	static const int HEIGHT = 256;
	static const int DEPTH = 16;
	static constexpr int VOLUME = WIDTH * HEIGHT * HEIGHT;

	const std::array<std::unique_ptr<Cube>, VOLUME>& GetBlocks() const noexcept;

private:
	std::array<std::unique_ptr<Cube>, VOLUME> m_blocks;
};

