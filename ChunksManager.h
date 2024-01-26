#pragma once

#include "DeviceResources.h"
#include "Chunk.h"
#include "WorldGenerator.h"
#include "ChunkRenderer.h"

struct LightNode
{
	LightNode(int x, int y, int z, std::shared_ptr<Chunk> chunk)
		: x(x), y(y), z(z), chunk(chunk)
	{}

	int x;
	int y;
	int z;
	std::shared_ptr<Chunk> chunk;
};

class ChunksManager
{
public:
	ChunksManager(std::unique_ptr<DX::DeviceResources>& deviceResources, DirectX::XMFLOAT3& playerPos);
	~ChunksManager();

	void ForEach(const std::function<void(std::pair<ChunkPos, std::shared_ptr<Chunk>>)> cb);
	void RemoveChunk(const ChunkPos& chunkPos);
	void InsertChunk(const ChunkPos& chunkPos, Chunk& chunk);
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
	std::shared_ptr<Chunk> GetChunkAt(ChunkPos& chunkPos);

private:
#ifdef NDEBUG
	static const int loadDistance = 32;
	static const int maxAsyncChunksLoading = 16;
	static const int maxAsyncChunksToUpdate = 8;
#else
	static const int loadDistance = 1;
	static const int maxAsyncChunksLoading = 4;
	static const int maxAsyncChunksToUpdate = 2;
#endif
	DirectX::XMFLOAT3 m_playerPos;
	std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash> m_chunks;
	std::unique_ptr<DX::DeviceResources>& m_deviceResources;
	std::unique_ptr<WorldGenerator> m_worldGenerator;
	std::unique_ptr<ChunkRenderer> m_chunkRenderer;
	std::queue<LightNode> m_redLightBfsQueue;
	std::queue<LightNode> m_greenLightBfsQueue;
	std::queue<LightNode> m_blueLightBfsQueue;
	std::queue<LightNode> m_sunlightBfsQueue;
	std::queue<LightNode> m_redLightRemovalBfsQueue;
	std::queue<LightNode> m_greenLightRemovalBfsQueue;
	std::queue<LightNode> m_blueLightRemovalBfsQueue;
	std::queue<LightNode> m_sunlightRemovalBfsQueue;
	BlockManager m_blockManager;

	std::shared_mutex m_mutex;
	std::thread m_thread;

	bool m_isRunning = true;
};

