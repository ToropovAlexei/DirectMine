#pragma once

#include "DeviceResources.h"
#include "Chunk.h"
#include "WorldGenerator.h"
#include "ChunkRenderer.h"
#include "Lighting.h"

class ChunksManager
{
friend LightSolver;
friend Lighting;
public:
	ChunksManager(std::unique_ptr<DX::DeviceResources>& deviceResources, DirectX::XMFLOAT3& playerPos);
	~ChunksManager();

	void RemoveChunk(int x, int z);
	void InsertChunk(Chunk& chunk);
	void UpdatePlayerPos(DirectX::XMFLOAT3& playerPos) noexcept;
	void RenderChunks();
	void RemoveBlockAt(WorldPos& worldPos);
	void PlaceBlockAt(WorldPos& worldPos, ChunkBlock block);
	bool CheckBlockCollision(WorldPos& worldPos);
	inline int ToChunkPos(int x) const noexcept
	{
		if (x >= 0)
		{
			return x / Chunk::WIDTH;
		}
		return (x - Chunk::WIDTH + 1) / Chunk::WIDTH;
	};

private:
	void AsyncProcessChunks();
	void LoadChunks();
	void CalculateLighting();
	void UpdateModifiedChunks();
	inline std::shared_ptr<Chunk> GetChunkAt(int x, int z) noexcept
	{
		const size_t idx = GetChunkIdx(x, z);
		return GetChunkAt(idx);
	}
	inline std::shared_ptr<Chunk> GetChunkAt(size_t idx) noexcept
	{
		if (idx >= m_chunks.size())
		{
			return nullptr;
		}
		return m_chunks[idx];
	}
	inline size_t GetChunkIdx(int x, int z) const noexcept
	{
		return (x - m_centerX + loadDistance) + (z - m_centerZ + loadDistance) * chunksArrSideSize;
	}
	inline size_t GetCenterIdx() const noexcept
	{
		return loadDistance + loadDistance * chunksArrSideSize;
	}

private:
#ifdef NDEBUG
	static const int loadDistance = 100;
	static const int maxAsyncChunksLoading = 4096;
	static const int maxAsyncChunksToUpdate = 256;
#else
	static const int loadDistance = 32;
	static const int maxAsyncChunksLoading = 4096;
	static const int maxAsyncChunksToUpdate = 256;
#endif
	static const int chunksArrSideSize = loadDistance * 2 + 1;
	int m_centerX;
	int m_centerZ;
	DirectX::XMFLOAT3 m_playerPos;
	std::vector<std::shared_ptr<Chunk>> m_chunks;
	std::unique_ptr<DX::DeviceResources>& m_deviceResources;
	std::unique_ptr<WorldGenerator> m_worldGenerator;
	std::unique_ptr<ChunkRenderer> m_chunkRenderer;
	std::unique_ptr<Lighting> m_lighting;
	BlockManager m_blockManager;

	std::shared_mutex m_mutex;
	std::thread m_thread;

	bool m_isRunning = true;
};

