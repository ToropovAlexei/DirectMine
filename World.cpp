#include "pch.h"
#include "World.h"

World::World(Camera* cam)
{
	m_chunks.push_back(std::make_unique<Chunk>());
	m_blockManager = BlockManager();
	m_blockManager.LoadBlocks();
}

void World::Update(float dt, Camera* cam)
{
}

const std::vector<std::unique_ptr<Chunk>>& World::Chunks() const noexcept
{
	return m_chunks;
}

void World::UpdateMesh(ID3D11Device* device)
{
	for (auto& chunk : m_chunks)
	{
		chunk->UpdateMesh(device, m_blockManager);
	}
}
