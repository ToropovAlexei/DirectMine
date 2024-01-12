#include "pch.h"
#include "World.h"
#include "TextureAtlas.h"
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

World::World(std::unique_ptr<DX::DeviceResources>& deviceResources, 
	std::unique_ptr<DirectX::Keyboard>& keyboard, 
	std::unique_ptr<DirectX::Mouse>& mouse, 
	std::unique_ptr<DirectX::Mouse::ButtonStateTracker>& tracker) :
	m_deviceResources(deviceResources),
	m_keyboard(keyboard),
	m_mouse(mouse),
	m_tracker(tracker)
{
	m_blockManager = BlockManager();
	m_blockManager.LoadBlocks();
	m_worldGenerator = std::make_unique<WorldGenerator>(m_blockManager);
	TextureAtlas::BuildAtlas(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
	m_cam = std::make_unique<Camera>();
	m_cam->UpdateViewMatrix();
	m_view = DirectX::XMMATRIX();
	m_proj = DirectX::XMMATRIX();
	m_chunkRenderer = std::make_unique<ChunkRenderer>(m_deviceResources);
	CreateMainCB();
}

void World::Update(DX::StepTimer const& timer)
{
	UpdateChunksToLoad();
	UpdateChunksToUnload();
	UnloadChunks();
	LoadChunks();
	UpdateChunksMesh();
	float elapsedTime = float(timer.GetElapsedSeconds());
	auto kb = m_keyboard->GetState();
	const float speed = 25.0f;
	if (kb.A)
	{
		m_cam->Strafe(-speed * elapsedTime);
	}
	if (kb.D)
	{
		m_cam->Strafe(speed * elapsedTime);
	}
	if (kb.W)
	{
		m_cam->Walk(speed * elapsedTime);
	}
	if (kb.S)
	{
		m_cam->Walk(-speed * elapsedTime);
	}

	auto mouse = m_mouse->GetState();
	m_tracker->Update(mouse);

	if (mouse.positionMode == DirectX::Mouse::MODE_RELATIVE)
	{
		float dx = DirectX::XMConvertToRadians(0.05f * static_cast<float>(mouse.x));
		float dy = DirectX::XMConvertToRadians(0.05f * static_cast<float>(mouse.y));
		m_cam->Pitch(dy);
		m_cam->RotateY(dx);
	}

	if (m_tracker->leftButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED)
	{
		m_mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	}
	else if (m_tracker->leftButton == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED)
	{
		m_mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
	}

	std::wstring title = L"DirectX 11 FPS: "
	+ std::to_wstring(timer.GetFramesPerSecond())
	+ L" x:" + std::to_wstring(static_cast<int>(m_cam->GetPosition3f().x))
	+ L" y:" + std::to_wstring(static_cast<int>(m_cam->GetPosition3f().y))
	+ L" z:" + std::to_wstring(static_cast<int>(m_cam->GetPosition3f().z));
	SetWindowText(GetActiveWindow(), title.c_str());

	m_cam->UpdateViewMatrix();
	m_view = m_cam->GetView();
	UpdateMainCB();
}

void World::Render()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	context->VSSetConstantBuffers(0u, 1u, m_mainCB.GetAddressOf());
	m_chunkRenderer->RenderChunks(m_chunks);
}

void World::OnWindowSizeChanged(float aspectRatio)
{
	m_proj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XM_PIDIV4,
		aspectRatio,
		0.1f, 1000.0f);
}

void World::CreateMainCB()
{
	const MainConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(m_view * m_proj)
		}
	};
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	m_deviceResources->GetD3DDevice()->CreateBuffer(&cbd, &csd, &m_mainCB);
}

void World::UpdateMainCB()
{
	D3D11_MAPPED_SUBRESOURCE msr;
	const MainConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(m_view * m_proj)
		}
	};
	auto context = m_deviceResources->GetD3DDeviceContext();
	context->Map(
		m_mainCB.Get(), 0u,
		D3D11_MAP_WRITE_DISCARD, 0u,
		&msr
	);
	memcpy(msr.pData, &cb, sizeof(cb));
	context->Unmap(m_mainCB.Get(), 0u);
}

bool World::HasChunkAt(ChunkPos& pos)
{
	return m_chunks.contains(pos);
}

void World::UpdateChunksToLoad()
{
	auto playerPos = m_cam->GetPosition3f();
	int xPos = static_cast<int>(std::round(playerPos.x / Chunk::WIDTH)) * Chunk::WIDTH;
	int zPos = static_cast<int>(std::round(playerPos.z / Chunk::DEPTH)) * Chunk::DEPTH;
	int offset = Chunk::WIDTH * chunkLoadingRadius;
	int xStart = xPos - offset;
	int zStart = zPos - offset;
	int xEnd = xPos + offset;
	int zEnd = zPos + offset;

	for (int x = xStart; x < xEnd; x += Chunk::WIDTH)
	{
		for (int z = zStart; z < zEnd; z += Chunk::WIDTH)
		{
			ChunkPos pos(x, z);
			if (!HasChunkAt(pos))
			{
				m_chunksToLoad.push_back(pos);
			}
		}
	}
}

void World::UpdateChunksToUnload()
{
	auto playerPos = m_cam->GetPosition3f();
	int playerX = static_cast<int>(playerPos.x);
	int playerZ = static_cast<int>(playerPos.z);
	int offset = Chunk::WIDTH * (chunkLoadingRadius + 2);
	for (auto& chunk : m_chunks)
	{
		int dx = abs(playerX - chunk.first.x);
		int dz = abs(playerZ - chunk.first.z);
		if (dx > offset || dz > offset)
		{
			m_chunksToUnload.push_back(chunk.first);
		}
	}
}

void World::UpdateChunksMesh()
{
	if (m_chunksToUpdateMesh.empty())
	{
		return;
	}
	auto start = std::chrono::high_resolution_clock::now();
	tbb::parallel_for(tbb::blocked_range<size_t>(0, m_chunksToUpdateMesh.size()),
		[this](const tbb::blocked_range<size_t>& range) {
			for (size_t i = range.begin(); i != range.end(); ++i) {
				m_chunks[m_chunksToUpdateMesh[i]]->UpdateMeshWithoutBuffers(m_blockManager);
			}
		});

	for (auto& pos : m_chunksToUpdateMesh)
	{
		m_chunks[pos]->UpdateBuffers(m_deviceResources->GetD3DDevice());
	}
	m_chunksToUpdateMesh.clear();
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::string result = "����� ���������� UpdateChunksMesh: " + std::to_string(duration.count()) + " �����������\n";
	OutputDebugStringA(result.c_str());
}

void World::LoadChunks()
{
	if (m_chunksToLoad.empty())
	{
		return;
	}
	auto start = std::chrono::high_resolution_clock::now();
	tbb::parallel_for(tbb::blocked_range<size_t>(0, m_chunksToLoad.size()),
		[this](const tbb::blocked_range<size_t>& range) {
			for (size_t i = range.begin(); i != range.end(); ++i) {
				auto chunk = m_worldGenerator->GenerateChunk(m_chunksToLoad[i]);
				mutex.lock();
				m_chunks.insert({ m_chunksToLoad[i], std::move(chunk) });
				m_chunksToUpdateMesh.push_back(m_chunksToLoad[i]);
				mutex.unlock();
			}
		});
	m_chunksToLoad.clear();
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::string result = "����� ���������� LoadChunks: " + std::to_string(duration.count()) + " �����������\n";
	OutputDebugStringA(result.c_str());
}

void World::UnloadChunks()
{
	for (auto& chunkToUnload : m_chunksToUnload)
	{
		m_chunks.erase(chunkToUnload);
	}
	m_chunksToUnload.clear();
}

