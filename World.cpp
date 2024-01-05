#include "pch.h"
#include "World.h"

World::World(Camera* cam)
{
	m_chunks.push_back(std::make_unique<Chunk>());
}

void World::Update(float dt, Camera* cam)
{
}

const std::vector<std::unique_ptr<Chunk>>& World::Chunks() const noexcept
{
	return m_chunks;
}
