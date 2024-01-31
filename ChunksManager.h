#pragma once

#include "DeviceResources.h"
#include "Chunk.h"
#include "WorldGenerator.h"
#include "ChunkRenderer.h"
#include "Lighting.h"

class ChunksManager
{
friend LightSolver;
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

private:
	void AsyncProcessChunks();
	void UnloadFarChunks();
	void LoadChunks();
	void CalculateLighting();
	void UpdateModifiedChunks();
	std::shared_ptr<Chunk> GetChunkAt(int x, int z);
	inline size_t GetChunkIdx(int x, int z);

private:
#ifdef NDEBUG
	static const int loadDistance = 50;
	static const int maxAsyncChunksLoading = 64;
	static const int maxAsyncChunksToUpdate = 32;
#else
	static const int loadDistance = 50;
	static const int maxAsyncChunksLoading = 64;
	static const int maxAsyncChunksToUpdate = 32;
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

