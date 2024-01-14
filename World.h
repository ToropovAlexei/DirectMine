#pragma once

#include "Camera.h"
#include "Chunk.h"
#include "BlockManager.h"
#include "DeviceResources.h"
#include "StepTimer.h"
#include "ChunkRenderer.h"
#include "WorldPos.hpp"
#include "WorldGenerator.h"
#include "BlockOutlineRenderer.h"
#include "CrosshairRenderer.h"

struct MainConstantBuffer
{
	DirectX::XMMATRIX viewProj;
};

class World
{
public:
	World(std::unique_ptr<DX::DeviceResources>& deviceResources, 
		std::unique_ptr<DirectX::Keyboard>& keyboard, 
		std::unique_ptr<DirectX::Mouse>& mouse, 
		std::unique_ptr<DirectX::Mouse::ButtonStateTracker>& tracker);

	void Update(DX::StepTimer const& timer);

	void Render();

	void OnWindowSizeChanged(float aspectRatio);

private:
	void CreateMainCB();
	void UpdateMainCB();

	bool HasChunkAt(ChunkPos& pos);

	void UpdateChunksToLoad();
	void UpdateChunksToUnload();
	void UpdateChunksMesh();

	void LoadChunks();
	void UnloadChunks();

	bool CheckBlockCollision(WorldPos& worldPos);

	void RemoveBlockAt(WorldPos& worldPos);

	std::optional<WorldPos> Raycast();
	
private:
#ifdef NDEBUG
	static const int chunkLoadingRadius = 24; // Release mode
#else
	static const int chunkLoadingRadius = 4; // Debug mode
#endif
	std::unordered_map<ChunkPos, std::unique_ptr<Chunk>, ChunkPosHash> m_chunks;
	std::vector<ChunkPos> m_chunksToLoad;
	std::vector<ChunkPos> m_chunksToUnload;
	std::vector<ChunkPos> m_chunksToUpdateMesh;
	std::unique_ptr<DX::DeviceResources>& m_deviceResources;
	std::unique_ptr<Camera> m_cam;
	std::unique_ptr<DirectX::Keyboard>& m_keyboard;
	std::unique_ptr<DirectX::Mouse>& m_mouse;
	std::unique_ptr<DirectX::Mouse::ButtonStateTracker>& m_tracker;

	BlockManager m_blockManager;
	std::unique_ptr<WorldGenerator> m_worldGenerator;
	std::unique_ptr<ChunkRenderer> m_chunkRenderer;
	std::unique_ptr<BlockOutlineRenderer> m_blockOutlineRenderer;
	std::unique_ptr<CrosshairRenderer> m_crosshairRenderer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_mainCB;

	DirectX::XMMATRIX m_view;
	DirectX::XMMATRIX m_proj;

	std::mutex mutex;
};

