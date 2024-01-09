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
	m_chunks.push_back(std::make_unique<Chunk>());
	m_chunks[0]->FillWith(); // TODO remove this
	m_chunks[0]->UpdateMesh(m_deviceResources->GetD3DDevice(), m_blockManager);
}

void World::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());
	auto kb = m_keyboard->GetState();
	const float speed = 5.0f;
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
		0.1f, 250.0f);
}

const std::vector<std::unique_ptr<Chunk>>& World::Chunks() const noexcept
{
	return m_chunks;
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

