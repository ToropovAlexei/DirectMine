#include "pch.h"
#include "ChunksManager.h"
#include "MathUtils.h"

ChunksManager::ChunksManager(std::unique_ptr<DX::DeviceResources>& deviceResources, DirectX::XMFLOAT3& playerPos) :
	m_playerPos(playerPos),
	m_deviceResources(deviceResources)
{
	m_blockManager = BlockManager();
	m_blockManager.LoadBlocks();
	m_worldGenerator = std::make_unique<WorldGenerator>(m_blockManager);
	m_chunkRenderer = std::make_unique<ChunkRenderer>(m_deviceResources);
	m_centerX = ToChunkPos(static_cast<int>(playerPos.x));
	m_centerZ = ToChunkPos(static_cast<int>(playerPos.z));
	m_chunks.resize(static_cast<size_t>(chunksArrSideSize * chunksArrSideSize));

	m_thread = std::thread([this]() {
		AsyncProcessChunks(); 
		});

	m_lighting = std::make_unique<Lighting>(this, m_blockManager);
}

ChunksManager::~ChunksManager()
{
	m_isRunning = false;
	m_thread.join();
}

void ChunksManager::RemoveChunk(int x, int z)
{
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	size_t chunkIdx = GetChunkIdx(x, z);
	m_chunks[chunkIdx] = nullptr;
	std::array<size_t, 4> nextChunks = { chunkIdx - 1, chunkIdx + 1, chunkIdx + chunksArrSideSize, chunkIdx - chunksArrSideSize };
	const size_t chunksCount = m_chunks.size();
	for (size_t nextChunkIdx : nextChunks)
	{
		if (nextChunkIdx < chunksCount && m_chunks[nextChunkIdx])
		{
			m_chunks[nextChunkIdx]->SetIsModified(true);
		}
	}
}

void ChunksManager::InsertChunk(std::unique_ptr<Chunk>& chunk)
{
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	size_t chunkIdx = GetChunkIdx(chunk->GetX(), chunk->GetZ());
	if (chunkIdx >= m_chunks.size())
	{
		return;
	}
	// TODO ���� ���� ������ ������, �� ������������ �����, ������-�� �� ������������� � std::move �����������
	m_chunks[chunkIdx] = std::move(chunk);
	std::array<size_t, 4> nextChunks = { chunkIdx - 1, chunkIdx + 1, chunkIdx + chunksArrSideSize, chunkIdx - chunksArrSideSize };
	const size_t chunksCount = m_chunks.size();
	for (size_t nextChunkIdx : nextChunks)
	{
		if (nextChunkIdx < chunksCount && m_chunks[nextChunkIdx])
		{
			m_chunks[nextChunkIdx]->SetIsModified(true);
		}
	}
}

void ChunksManager::UpdatePlayerPos(DirectX::XMFLOAT3& playerPos) noexcept
{
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_playerPos = playerPos;
	int newX = ToChunkPos(static_cast<int>(playerPos.x));
	int newZ = ToChunkPos(static_cast<int>(playerPos.z));
	int dX = m_centerX - newX;
	int dZ = m_centerZ - newZ;
	m_centerX = newX;
	m_centerZ = newZ;
	if (dX || dZ)
	{
		std::vector<std::shared_ptr<Chunk>> newChunks(m_chunks.size());
		int minX = std::max(0, -dX);
		int maxX = std::min(chunksArrSideSize, chunksArrSideSize - dX);
		size_t zIdx = std::max(0, -dZ) * chunksArrSideSize;
		for (int z = std::max(0, -dZ); z < std::min(chunksArrSideSize, chunksArrSideSize - dZ); z++)
		{
			size_t zOffset = (z + dZ) * chunksArrSideSize;
			for (int x = minX; x < maxX; x++)
			{
				size_t newIdx = static_cast<size_t>(x + dX + zOffset);
				if (newIdx < m_chunks.size())
				{
					newChunks[newIdx] = m_chunks[zIdx + static_cast<size_t>(x)];
					//if (newChunks[newIdx] && ((z + dZ) == 0 || (z + dZ) == chunksArrSideSize - 1 || (x + dX) == 0 || (x + dX) == chunksArrSideSize - 1))
					//{
					//	newChunks[newIdx]->SetIsModified(true);
					//}
				}
			}
			zIdx += static_cast<size_t>(chunksArrSideSize);
		}
		std::swap(newChunks, m_chunks);
	}
}

void ChunksManager::RenderChunks()
{
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_chunkRenderer->RenderChunks(m_chunks);
}

void ChunksManager::RemoveBlockAt(WorldPos& worldPos)
{
	PlaceBlockAt(worldPos, ChunkBlock(BlockId::Air));
}

void ChunksManager::PlaceBlockAt(WorldPos& worldPos, ChunkBlock block)
{
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	int xPos = ToChunkPos(worldPos.x);
	int zPos = ToChunkPos(worldPos.z);
	size_t chunkIdx = GetChunkIdx(xPos, zPos);
	if (chunkIdx >= m_chunks.size())
	{
		return;
	}
	auto& chunk = m_chunks[chunkIdx];
	if (!chunk)
	{
		return;
	}
	int blockX = worldPos.x - xPos * Chunk::WIDTH;
	int blockZ = worldPos.z - zPos * Chunk::WIDTH;
	chunk->SetBlock(blockX, worldPos.y, blockZ, block);
	m_lighting->HandleBlockSet(blockX, worldPos.y, blockZ, chunk, block);
	chunk->SetIsModified(true);
	m_lighting->BuildSunlight(chunk);

	if (blockX == 0)
	{
		size_t idx = chunkIdx - 1;
		if (idx < m_chunks.size() && m_chunks[idx])
		{
			m_chunks[idx]->SetIsModified(true);
		}
	}
	if (blockX == Chunk::LAST_BLOCK_IDX)
	{
		size_t idx = chunkIdx + 1;
		if (idx < m_chunks.size() && m_chunks[idx])
		{
			m_chunks[idx]->SetIsModified(true);
		}
	}
	if (blockZ == 0)
	{
		size_t idx = chunkIdx - chunksArrSideSize;
		if (idx < m_chunks.size() && m_chunks[idx])
		{
			m_chunks[idx]->SetIsModified(true);
		}
	}
	if (blockZ == Chunk::LAST_BLOCK_IDX)
	{
		size_t idx = chunkIdx + chunksArrSideSize;
		if (idx < m_chunks.size() && m_chunks[idx])
		{
			m_chunks[idx]->SetIsModified(true);
		}
	}
}

bool ChunksManager::CheckBlockCollision(WorldPos& worldPos)
{
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	int xPos = ToChunkPos(worldPos.x);
	int zPos = ToChunkPos(worldPos.z);
	auto chunk = GetChunkAt(xPos, zPos);
	if (!chunk)
	{
		return false;
	}
	return chunk->HasBlockAt(worldPos.x - xPos * Chunk::WIDTH, worldPos.y, worldPos.z - zPos * Chunk::WIDTH);
}

void ChunksManager::AsyncProcessChunks()
{
	while (m_isRunning)
	{
		LoadChunks();
		CalculateLighting();
		UpdateModifiedChunks();
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

void ChunksManager::LoadChunks()
{
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	std::vector<std::pair<int, int>> chunksToLoad;
	const size_t centerIdx = GetCenterIdx();

	if (m_chunks[centerIdx] == nullptr)
	{
		chunksToLoad.push_back({m_centerX, m_centerZ});
	}

	int radius = 1;
	while (radius <= loadDistance && chunksToLoad.size() < maxAsyncChunksLoading)
	{
		int xStart = m_centerX - radius;
		int xEnd = m_centerX + radius;
		int zStart = m_centerZ - radius;
		int zEnd = m_centerZ + radius;

		for (int x = xStart; x <= xEnd; x++)
		{
			if (chunksToLoad.size() >= maxAsyncChunksLoading)
			{
				break;
			}
			if (!GetChunkAt(x, zStart))
			{
				chunksToLoad.push_back({x, zStart});
			}
			if (!GetChunkAt(x, zEnd))
			{
				chunksToLoad.push_back({x, zEnd});
			}
		}
		for (int z = zStart + 1; z <= zEnd - 1; z++)
		{
			if (chunksToLoad.size() >= maxAsyncChunksLoading)
			{
				break;
			}
			if (!GetChunkAt(xStart, z))
			{
				chunksToLoad.push_back({xStart, z});
			}
			if (!GetChunkAt(xEnd, z))
			{
				chunksToLoad.push_back({xEnd, z});
			}
		}
		radius++;
	}
	lock.unlock();

	std::for_each(std::execution::par, chunksToLoad.begin(), chunksToLoad.end(), [this](std::pair<int, int>& coords) {
		auto chunk = m_worldGenerator->GenerateChunk(coords.first, coords.second);
		chunk->UpdateSunlight(m_blockManager);
		InsertChunk(chunk);
	});
}

void ChunksManager::CalculateLighting()
{
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_lighting->Solve();
}

void ChunksManager::UpdateModifiedChunks()
{
	std::vector<std::shared_ptr<Chunk>> chunksToUpdate;
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	const size_t centerIdx = GetCenterIdx();

	if (m_chunks[centerIdx] && m_chunks[centerIdx]->IsModified())
	{
		chunksToUpdate.push_back(m_chunks[centerIdx]);
	}

	int radius = 1;
	while (radius <= loadDistance && chunksToUpdate.size() < maxAsyncChunksToUpdate)
	{
		int xStart = m_centerX - radius;
		int xEnd = m_centerX + radius;
		int zStart = m_centerZ - radius;
		int zEnd = m_centerZ + radius;

		for (int x = xStart; x <= xEnd; x++)
		{
			if (chunksToUpdate.size() >= maxAsyncChunksToUpdate)
			{
				break;
			}
			auto frontChunk = GetChunkAt(x, zStart);
			auto backChunk = GetChunkAt(x, zEnd);
			if (frontChunk && frontChunk->IsModified())
			{
				chunksToUpdate.push_back(frontChunk);
			}
			if (backChunk && backChunk->IsModified())
			{
				chunksToUpdate.push_back(backChunk);
			}
		}
		for (int z = zStart + 1; z <= zEnd - 1; z++)
		{
			if (chunksToUpdate.size() >= maxAsyncChunksToUpdate)
			{
				break;
			}
			auto leftChunk = GetChunkAt(xStart, z);
			auto rightChunk = GetChunkAt(xEnd, z);
			if (leftChunk && leftChunk->IsModified())
			{
				chunksToUpdate.push_back(leftChunk);
			}
			if (rightChunk && rightChunk->IsModified())
			{
				chunksToUpdate.push_back(rightChunk);
			}
		}
		radius++;
	}

	std::for_each(std::execution::par, chunksToUpdate.begin(), chunksToUpdate.end(), [this](std::shared_ptr<Chunk> chunk) {
		const size_t idx = GetChunkIdx(chunk->GetX(), chunk->GetZ());
		auto leftChunk = GetChunkAt(idx - 1);
		auto rightChunk = GetChunkAt(idx + 1);
		auto frontChunk = GetChunkAt(idx - chunksArrSideSize);
		auto backChunk = GetChunkAt(idx + chunksArrSideSize);
		chunk->UpdateMeshWithoutBuffers(m_blockManager,
			leftChunk, rightChunk,
			frontChunk, backChunk);
		chunk->UpdateBuffers(m_deviceResources->GetD3DDevice());
		chunk->SetShouldRender(true);
		chunk->SetIsModified(false);
		});
}
