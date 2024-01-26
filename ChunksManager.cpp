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
	m_lighting = std::make_unique<Lighting>(m_chunks, m_blockManager);
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
	ChunkPos leftChunk = chunkPos - ChunkPos(Chunk::WIDTH, 0);
	ChunkPos rightChunk = chunkPos + ChunkPos(Chunk::WIDTH, 0);
	ChunkPos frontChunk = chunkPos - ChunkPos(0, Chunk::WIDTH);
	ChunkPos backChunk = chunkPos + ChunkPos(0, Chunk::WIDTH);
	std::array<ChunkPos, 4> chunks = {leftChunk, rightChunk, frontChunk, backChunk};
	for (auto& nextChunkPos : chunks)
	{
		auto it = m_chunks.find(nextChunkPos);
		if (it == m_chunks.end())
		{
			continue;
		}
		it->second->SetIsModified(true);
	}
}

void ChunksManager::UpdatePlayerPos(DirectX::XMFLOAT3& playerPos) noexcept
{
	m_playerPos = playerPos;
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
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	int xPos = MathUtils::RoundDown(static_cast<int>(worldPos.x), Chunk::WIDTH);
	int zPos = MathUtils::RoundDown(static_cast<int>(worldPos.z), Chunk::WIDTH);
	ChunkPos chunkPos = ChunkPos(xPos, zPos);
	auto it = m_chunks.find(chunkPos);
	if (it == m_chunks.end())
	{
		return;
	}
	WorldPos blockPos = WorldPos(worldPos.x - xPos, worldPos.y, worldPos.z - zPos);
	auto leftChunk = m_chunks.find(chunkPos + ChunkPos(-1, 0));
	auto rightChunk = m_chunks.find(chunkPos + ChunkPos(1, 0));
	auto frontChunk = m_chunks.find(chunkPos + ChunkPos(0, -1));
	auto backChunk = m_chunks.find(chunkPos + ChunkPos(0, 1));
	it->second->SetBlock(blockPos.x, blockPos.y, blockPos.z, block);
	auto& emission = m_blockManager.GetBlockById(block.GetId()).GetEmission();
	if (emission[0] || emission[1] || emission[2])
	{
		it->second->GetLightmapRef().SetR(blockPos.x, blockPos.y, blockPos.z, std::max(it->second->GetLightmapRef().GetR(blockPos.x, blockPos.y, blockPos.z), static_cast<int>(emission[0])));
		it->second->GetLightmapRef().SetG(blockPos.x, blockPos.y, blockPos.z, std::max(it->second->GetLightmapRef().GetG(blockPos.x, blockPos.y, blockPos.z), static_cast<int>(emission[1])));
		it->second->GetLightmapRef().SetB(blockPos.x, blockPos.y, blockPos.z, std::max(it->second->GetLightmapRef().GetB(blockPos.x, blockPos.y, blockPos.z), static_cast<int>(emission[2])));
		m_lighting->solverR->Add({ blockPos.x, blockPos.y, blockPos.z, it->second });
		m_lighting->solverG->Add({ blockPos.x, blockPos.y, blockPos.z, it->second });
		m_lighting->solverB->Add({ blockPos.x, blockPos.y, blockPos.z, it->second });
	}
	it->second->SetIsModified(true);
	// Возникает какой-то глитч с отображением чанка
	//it->second->UpdateMeshWithoutBuffers(m_blockManager,
	//	leftChunk == m_chunks.end() ? nullptr : leftChunk->second, rightChunk == m_chunks.end() ? nullptr : rightChunk->second,
	//	frontChunk == m_chunks.end() ? nullptr : frontChunk->second, backChunk == m_chunks.end() ? nullptr : backChunk->second);
	//it->second->UpdateBuffers(m_deviceResources->GetD3DDevice());

	if (blockPos.x == 0)
	{
		ChunkPos leftChunk = chunkPos - ChunkPos(Chunk::WIDTH, 0);
		auto chunk = GetChunkAt(leftChunk);
		if (chunk != nullptr)
		{
			chunk->SetIsModified(true);
		}
	}
	if (blockPos.x == Chunk::LAST_BLOCK_IDX)
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
		ChunkPos frontChunk = chunkPos - ChunkPos(0, Chunk::WIDTH);
		auto chunk = GetChunkAt(frontChunk);
		if (chunk != nullptr)
		{
			chunk->SetIsModified(true);
		}
	}
	if (blockPos.z == Chunk::LAST_BLOCK_IDX)
	{
		ChunkPos backChunk = chunkPos + ChunkPos(0, Chunk::WIDTH);
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
	int zPos = MathUtils::RoundDown(static_cast<int>(worldPos.z), Chunk::WIDTH);
	ChunkPos chunkPos = ChunkPos(xPos, zPos);
	auto it = m_chunks.find(chunkPos);
	if (it == m_chunks.end())
	{
		return false;
	}
	return it->second->HasBlockAt(worldPos.x - xPos, worldPos.y, worldPos.z - zPos);
}

void ChunksManager::AsyncProcessChunks()
{
	while (m_isRunning)
	{
		UnloadFarChunks();
		LoadChunks();
		CalculateLighting();
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
	int xCenter = static_cast<int>(std::round(m_playerPos.x / Chunk::WIDTH)) * Chunk::WIDTH;
	int zCenter = static_cast<int>(std::round(m_playerPos.z / Chunk::WIDTH)) * Chunk::WIDTH;

	std::vector<ChunkPos> chunksToLoad;

	int radius = 0;
	while (radius < loadDistance && chunksToLoad.size() < maxAsyncChunksLoading)
	{
		if (radius == 0)
		{
			ChunkPos pos(xCenter, zCenter);
			if (!m_chunks.contains(pos))
			{
				chunksToLoad.push_back(pos);
			}
			radius++;
		}
		int xStart = xCenter - radius * Chunk::WIDTH;
		int xEnd = xCenter + radius * Chunk::WIDTH;
		int zStart = zCenter - radius * Chunk::WIDTH;
		int zEnd = zCenter + radius * Chunk::WIDTH;

		for (int x = xStart; x <= xEnd; x += Chunk::WIDTH)
		{
			if (chunksToLoad.size() >= maxAsyncChunksLoading)
			{
				break;
			}
			ChunkPos posFront(x, zStart);
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (!m_chunks.contains(posFront))
			{
				chunksToLoad.push_back(posFront);
			}
			ChunkPos posBack(x, zEnd);
			if (!m_chunks.contains(posBack))
			{
				chunksToLoad.push_back(posBack);
			}
		}
		for (int z = zStart + Chunk::WIDTH; z <= zEnd - Chunk::WIDTH; z += Chunk::WIDTH)
		{
			if (chunksToLoad.size() >= maxAsyncChunksLoading)
			{
				break;
			}
			ChunkPos posLeft(xStart, z);
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (!m_chunks.contains(posLeft))
			{
				chunksToLoad.push_back(posLeft);
			}
			ChunkPos posRight(xEnd, z);
			if (!m_chunks.contains(posRight))
			{
				chunksToLoad.push_back(posRight);
			}
		}
		radius++;
	}

	tbb::parallel_for(tbb::blocked_range<size_t>(0, chunksToLoad.size()),
		[this, &chunksToLoad](const tbb::blocked_range<size_t>& range) {
			for (size_t i = range.begin(); i != range.end(); ++i) {
				auto chunk = m_worldGenerator->GenerateChunk(chunksToLoad[i]);
				InsertChunk(chunksToLoad[i], chunk);
			}
		});
}

void ChunksManager::CalculateLighting()
{
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_lighting->solverS->Solve();
	m_lighting->solverR->Solve();
	m_lighting->solverG->Solve();
	m_lighting->solverB->Solve();
}

void ChunksManager::UpdateModifiedChunks()
{
	int playerX = static_cast<int>(m_playerPos.x);
	int playerZ = static_cast<int>(m_playerPos.z);

	auto comp = [&playerX, &playerZ](ChunkPos left, ChunkPos right) {
		double dist1 = std::pow(playerX - left.x, 2) + std::pow(playerZ - left.z, 2);
		double dist2 = std::pow(playerX - right.x, 2) + std::pow(playerZ - right.z, 2);

		return dist1 > dist2;
		};
	std::priority_queue<ChunkPos, std::vector<ChunkPos>, decltype(comp)> prior(comp);
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	for (auto& chunkPair : m_chunks)
	{
		if (chunkPair.second->IsModified())
		{
			prior.push(chunkPair.first);
		}
	}
	std::vector<ChunkPos> modifiedChunks;
	while (!prior.empty() && modifiedChunks.size() < maxAsyncChunksToUpdate)
	{
		modifiedChunks.push_back(prior.top());
		prior.pop();
	}

	tbb::parallel_for(tbb::blocked_range<size_t>(0, modifiedChunks.size()),
		[this, &modifiedChunks](const tbb::blocked_range<size_t>& range) {
			for (size_t i = range.begin(); i != range.end(); ++i) {
				ChunkPos chunkPos = m_chunks[modifiedChunks[i]]->GetPos();
				auto leftChunk = m_chunks.find(chunkPos + ChunkPos(-Chunk::WIDTH, 0));
				auto rightChunk = m_chunks.find(chunkPos + ChunkPos(Chunk::WIDTH, 0));
				auto frontChunk = m_chunks.find(chunkPos + ChunkPos(0, -Chunk::WIDTH));
				auto backChunk = m_chunks.find(chunkPos + ChunkPos(0, Chunk::WIDTH));
				m_chunks[modifiedChunks[i]]->UpdateMeshWithoutBuffers(m_blockManager, 
					leftChunk == m_chunks.end() ? nullptr : leftChunk->second, rightChunk == m_chunks.end() ? nullptr : rightChunk->second,
					frontChunk == m_chunks.end() ? nullptr : frontChunk->second, backChunk == m_chunks.end() ? nullptr : backChunk->second);
			}
		});

	for (auto& pos : modifiedChunks)
	{
		m_chunks[pos]->UpdateBuffers(m_deviceResources->GetD3DDevice());
		m_chunks[pos]->SetShouldRender(true);
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
