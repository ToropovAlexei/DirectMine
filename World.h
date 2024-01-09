#pragma once

#include "Camera.h"
#include "Chunk.h"
#include "BlockManager.h"
#include "DeviceResources.h"
#include "StepTimer.h"
#include "ChunkRenderer.h"
#include "WorldPos.hpp"

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

private:
	std::unordered_map<WorldPos, std::unique_ptr<Chunk>, WorldPosHash> m_chunks;
	std::unique_ptr<DX::DeviceResources>& m_deviceResources;
	std::unique_ptr<Camera> m_cam;
	std::unique_ptr<DirectX::Keyboard>& m_keyboard;
	std::unique_ptr<DirectX::Mouse>& m_mouse;
	std::unique_ptr<DirectX::Mouse::ButtonStateTracker>& m_tracker;

	BlockManager m_blockManager;
	std::unique_ptr<ChunkRenderer> m_chunkRenderer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_mainCB;

	DirectX::XMMATRIX m_view;
	DirectX::XMMATRIX m_proj;
};

