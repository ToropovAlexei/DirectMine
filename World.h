#pragma once

#include "Camera.h"
#include "Chunk.h"
#include "BlockManager.h"

class World
{
public:
	World(Camera* cam);

	void Update(float dt, Camera* cam);

	const std::vector<std::unique_ptr<Chunk>>& Chunks() const noexcept;
	void UpdateMesh(ID3D11Device* device);

private:
	std::vector<std::unique_ptr<Chunk>> m_chunks;
	BlockManager m_blockManager;
};

