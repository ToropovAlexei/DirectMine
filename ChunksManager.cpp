#include "pch.h"
#include "ChunksManager.h"
#include <tbb/parallel_for.h>
#include "MathUtils.h"

ChunksManager::ChunksManager(std::unique_ptr<DX::DeviceResources>& deviceResources, DirectX::XMFLOAT3& playerPos) :
	m_deviceResources(deviceResources),
	m_playerPos(playerPos)
{
	m_blockManager = BlockManager();
	m_blockManager.LoadBlocks();
	m_worldGenerator = std::make_unique<WorldGenerator>(m_blockManager);
	m_chunkRenderer = std::make_unique<ChunkRenderer>(m_deviceResources);
	m_thread = std::thread([this]() {
		AsyncProcessChunks(); 
		});
}

ChunksManager::~ChunksManager()
{
	m_isRunning = false;
	m_thread.join();
}

void ChunksManager::ForEach(const std::function<void(std::pair<ChunkPos, std::shared_ptr<Chunk>>)> cb)
{
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	std::for_each(m_chunks.begin(), m_chunks.end(), cb);
}

void ChunksManager::RemoveChunk(const ChunkPos& chunkPos)
{
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_chunks.erase(chunkPos);
}

void ChunksManager::InsertChunk(const ChunkPos& chunkPos, Chunk& chunk)
{
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_chunks.insert({ chunkPos, std::make_shared<Chunk>(chunk) });
}

void ChunksManager::UpdatePlayerPos(DirectX::XMFLOAT3& playerPos) noexcept
{
	m_playerPos = playerPos;
}

void ChunksManager::RenderChunks()
{
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	m_chunkRenderer->RenderChunks(m_chunks);
}

void ChunksManager::RemoveBlockAt(WorldPos& worldPos)
{
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	int xPos = MathUtils::RoundDown(static_cast<int>(worldPos.x), Chunk::WIDTH);
	int zPos = MathUtils::RoundDown(static_cast<int>(worldPos.z), Chunk::DEPTH);
	ChunkPos chunkPos = ChunkPos(xPos, zPos);
	auto it = m_chunks.find(chunkPos);
	if (it == m_chunks.end())
	{
		return;
	}
	WorldPos blockPos = WorldPos(worldPos.x - xPos, worldPos.y, worldPos.z - zPos);
	it->second->RemoveBlock(blockPos);
	it->second->UpdateMeshWithoutBuffers(m_blockManager, m_chunks);
	it->second->UpdateBuffers(m_deviceResources->GetD3DDevice());

	if (blockPos.x == 0)
	{
		ChunkPos leftChunk = chunkPos - ChunkPos(Chunk::WIDTH, 0);
		auto chunk = GetChunkAt(leftChunk);
		if (chunk != nullptr)
		{
			chunk->SetIsModified(true);
		}
	}
	if (blockPos.x == Chunk::WIDTH - 1)
	{
		ChunkPos rightChunk = chunkPos + ChunkPos(Chunk::WIDTH, 0);
		auto chunk = GetChunkAt(rightChunk);
		if (chunk != nullptr)
		{
			chunk->SetIsModified(true);
		}
	}
	if (blockPos.z == 0)
	{
		ChunkPos frontChunk = chunkPos - ChunkPos(0, Chunk::DEPTH);
		auto chunk = GetChunkAt(frontChunk);
		if (chunk != nullptr)
		{
			chunk->SetIsModified(true);
		}
	}
	if (blockPos.z == Chunk::DEPTH - 1)
	{
		ChunkPos backChunk = chunkPos + ChunkPos(0, Chunk::DEPTH);
		auto chunk = GetChunkAt(backChunk);
		if (chunk != nullptr)
		{
			chunk->SetIsModified(true);
		}
	}
}

void ChunksManager::PlaceBlockAt(WorldPos& worldPos, ChunkBlock block)
{
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	int xPos = MathUtils::RoundDown(static_cast<int>(worldPos.x), Chunk::WIDTH);
	int zPos = MathUtils::RoundDown(static_cast<int>(worldPos.z), Chunk::DEPTH);
	ChunkPos chunkPos = ChunkPos(xPos, zPos);
	auto it = m_chunks.find(chunkPos);
	if (it == m_chunks.end())
	{
		return;
	}
	WorldPos blockPos = WorldPos(worldPos.x - xPos, worldPos.y, worldPos.z - zPos);
	it->second->AddBlock(blockPos, block);
	it->second->UpdateMeshWithoutBuffers(m_blockManager, m_chunks);
	it->second->UpdateBuffers(m_deviceResources->GetD3DDevice());

	if (blockPos.x == 0)
	{
		ChunkPos leftChunk = chunkPos - ChunkPos(Chunk::WIDTH, 0);
		auto chunk = GetChunkAt(leftChunk);
		if (chunk != nullptr)
		{
			chunk->SetIsModified(true);
		}
	}
	if (blockPos.x == Chunk::WIDTH - 1)
	{
		ChunkPos rightChunk = chunkPos + ChunkPos(Chunk::WIDTH, 0);
		auto chunk = GetChunkAt(rightChunk);
		if (chunk != nullptr)
		{
			chunk->SetIsModified(true);
		}
	}
	if (blockPos.z == 0)
	{
		ChunkPos frontChunk = chunkPos - ChunkPos(0, Chunk::DEPTH);
		auto chunk = GetChunkAt(frontChunk);
		if (chunk != nullptr)
		{
			chunk->SetIsModified(true);
		}
	}
	if (blockPos.z == Chunk::DEPTH - 1)
	{
		ChunkPos backChunk = chunkPos + ChunkPos(0, Chunk::DEPTH);
		auto chunk = GetChunkAt(backChunk);
		if (chunk != nullptr)
		{
			chunk->SetIsModified(true);
		}
	}
}

bool ChunksManager::CheckBlockCollision(WorldPos& worldPos)
{
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	int xPos = MathUtils::RoundDown(static_cast<int>(worldPos.x), Chunk::WIDTH);
	int zPos = MathUtils::RoundDown(static_cast<int>(worldPos.z), Chunk::DEPTH);
	ChunkPos chunkPos = ChunkPos(xPos, zPos);
	auto it = m_chunks.find(chunkPos);
	if (it == m_chunks.end())
	{
		return false;
	}
	WorldPos blockPos = WorldPos(worldPos.x - xPos, worldPos.y, worldPos.z - zPos);
	return it->second->HasBlockAt(blockPos);
}

void ChunksManager::AsyncProcessChunks()
{
	while (m_isRunning)
	{
		UnloadFarChunks();
		LoadChunks();
		UpdateModifiedChunks();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void ChunksManager::UnloadFarChunks()
{
	int playerX = static_cast<int>(m_playerPos.x);
	int playerZ = static_cast<int>(m_playerPos.z);
	int offset = Chunk::WIDTH * (loadDistance + 2);

	std::vector<ChunkPos> chunksToUnload;

	std::shared_lock<std::shared_mutex> lock(m_mutex);
	for (auto& chunk : m_chunks)
	{
		int dx = abs(playerX - chunk.first.x);
		int dz = abs(playerZ - chunk.first.z);
		if (dx > offset || dz > offset)
		{
			chunksToUnload.push_back(chunk.first);
		}
	}
	lock.unlock();

	for (auto& chunkPos : chunksToUnload)
	{
		RemoveChunk(chunkPos);
	}
}

void ChunksManager::LoadChunks()
{
	int xPos = static_cast<int>(std::round(m_playerPos.x / Chunk::WIDTH)) * Chunk::WIDTH;
	int zPos = static_cast<int>(std::round(m_playerPos.z / Chunk::DEPTH)) * Chunk::DEPTH;
	int offset = Chunk::WIDTH * loadDistance;
	int xStart = xPos - offset;
	int zStart = zPos - offset;
	int xEnd = xPos + offset;
	int zEnd = zPos + offset;

	std::vector<ChunkPos> chunksToLoad;

	for (int x = xStart; x < xEnd; x += Chunk::WIDTH)
	{
		for (int z = zStart; z < zEnd; z += Chunk::WIDTH)
		{
			ChunkPos pos(x, z);
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (!m_chunks.contains(pos))
			{
				chunksToLoad.push_back(pos);
			}
			if (chunksToLoad.size() == maxAsyncChunksLoading)
			{
				break;
			}
		}
		if (chunksToLoad.size() == maxAsyncChunksLoading)
		{
			break;
		}
	}
	tbb::parallel_for(tbb::blocked_range<size_t>(0, chunksToLoad.size()),
		[this, &chunksToLoad](const tbb::blocked_range<size_t>& range) {
			for (size_t i = range.begin(); i != range.end(); ++i) {
				auto chunk = m_worldGenerator->GenerateChunk(chunksToLoad[i]);
				InsertChunk(chunksToLoad[i], chunk);
			}
		});
}

void ChunksManager::UpdateModifiedChunks()
{
	std::vector<ChunkPos> modifiedChunks;
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	for (auto& chunkPair : m_chunks)
	{
		if (chunkPair.second->IsModified())
		{
			modifiedChunks.push_back(chunkPair.first);
		}
		if (modifiedChunks.size() == maxAsyncChunksLoading)
		{
			break;
		}
	}

	tbb::parallel_for(tbb::blocked_range<size_t>(0, modifiedChunks.size()),
		[this, &modifiedChunks](const tbb::blocked_range<size_t>& range) {
			for (size_t i = range.begin(); i != range.end(); ++i) {
				m_chunks[modifiedChunks[i]]->UpdateMeshWithoutBuffers(m_blockManager, m_chunks);
			}
		});

	for (auto& pos : modifiedChunks)
	{
		m_chunks[pos]->UpdateBuffers(m_deviceResources->GetD3DDevice());
		m_chunks[pos]->SetIsModified(false);
	}
}

std::shared_ptr<Chunk> ChunksManager::GetChunkAt(ChunkPos& chunkPos)
{
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	auto it = m_chunks.find(chunkPos);
	if (it == m_chunks.end())
	{
		return nullptr;
	}
	return it->second;
}
