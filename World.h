#pragma once

#include "Camera.h"
#include "Chunk.h"

class World
{
public:
	World(Camera* cam);

	void Update(float dt, Camera* cam);

	const std::vector<std::unique_ptr<Chunk>>& Chunks() const noexcept;

private:
	std::vector<std::unique_ptr<Chunk>> m_chunks;
};

