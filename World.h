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
#include "ChunksManager.h"

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
		std::unique_ptr<DirectX::Mouse::ButtonStateTracker>& tracker,
		std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker>& keysTracker);

	void Update(DX::StepTimer const& timer);

	void Render();

	void OnWindowSizeChanged(float aspectRatio);

private:
	void CreateMainCB();
	void UpdateMainCB();

	std::optional<std::pair<WorldPos, ChunkBlock::BlockDirection>> Raycast();
	
private:
	std::unique_ptr<DX::DeviceResources>& m_deviceResources;
	std::unique_ptr<Camera> m_cam;
	std::unique_ptr<DirectX::Keyboard>& m_keyboard;
	std::unique_ptr<DirectX::Mouse>& m_mouse;
	std::unique_ptr<DirectX::Mouse::ButtonStateTracker>& m_tracker;
	std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker>& m_keysTracker;

	BlockManager m_blockManager;
	std::unique_ptr<WorldGenerator> m_worldGenerator;
	std::unique_ptr<BlockOutlineRenderer> m_blockOutlineRenderer;
	std::unique_ptr<CrosshairRenderer> m_crosshairRenderer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_mainCB;
	std::unique_ptr<ChunksManager> m_chunksManager;

	DirectX::XMMATRIX m_view;
	DirectX::XMMATRIX m_proj;

	std::mutex mutex;
};

