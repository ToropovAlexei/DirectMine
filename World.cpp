#include "pch.h"
#include "World.h"
#include "TextureAtlas.h"

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
	TextureAtlas::BuildAtlas(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
	m_cam = std::make_unique<Camera>();
	m_cam->UpdateViewMatrix();
	m_view = DirectX::XMMATRIX();
	m_proj = DirectX::XMMATRIX();
	m_chunkRenderer = std::make_unique<ChunkRenderer>(m_deviceResources);
	CreateMainCB();

	int numThreads = 4;
	std::vector<std::thread> threads;

	for (int i = 0; i < numThreads; i++)
	{
		threads.emplace_back([this, i]() {
			TEST_ADD_CHUNK(0, 0, i * 4, 8, 1, i * 4 + 4);
			});
	}
	if (threads.size() >= numThreads) {
		for (std::thread& thread : threads) {
			thread.join();
		}
		threads.clear();
	}

	for (std::thread& thread : threads) {
		thread.join();
	}

	for (auto& chunk : m_chunks)
	{
		chunk.second->UpdateBuffers(m_deviceResources->GetD3DDevice());
	}

	//for (int x = 0; x < 16; x++)
	//{
	//	for (int y = 0; y < 1; y++)
	//	{
	//		for (int z = 0; z < 16; z++)
	//		{
	//			WorldPos chunkPos = { 32.0f * x, 32.0f * y, 32.0f * z };
	//			m_chunks.insert({ chunkPos, std::make_unique<Chunk>(chunkPos) });
	//			m_chunks[chunkPos]->FillWith(); // TODO remove this
	//			m_chunks[chunkPos]->UpdateMesh(m_deviceResources->GetD3DDevice(), m_blockManager);
	//		}
	//	}
	//}
}

void World::Update(DX::StepTimer const& timer)
{
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

void World::TEST_ADD_CHUNK(int x1, int y1, int z1, int x2, int y2, int z2)
{
	std::unordered_map<WorldPos, std::unique_ptr<Chunk>, WorldPosHash> generatedChunks;
	for (int x = x1; x < x2; x++)
	{
		for (int y = y1; y < y2; y++)
		{
			for (int z = z1; z < z2; z++)
			{
				WorldPos chunkPos = { 32.0f * x, 32.0f * y, 32.0f * z };
				generatedChunks.insert({ chunkPos, std::make_unique<Chunk>(chunkPos) });
				generatedChunks[chunkPos]->FillWith(); // TODO remove this
				generatedChunks[chunkPos]->UpdateMeshWithoutBuffers(m_blockManager);
			}
		}
	}
	std::lock_guard<std::mutex> lock(mutex);
	m_chunks.insert(std::make_move_iterator(generatedChunks.begin()), std::make_move_iterator(generatedChunks.end()));
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

