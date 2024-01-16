#include "pch.h"
#include "World.h"
#include "TextureAtlas.h"
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include "MathUtils.h"
#include "WorldUtils.h"

World::World(std::unique_ptr<DX::DeviceResources>& deviceResources, 
	std::unique_ptr<DirectX::Keyboard>& keyboard, 
	std::unique_ptr<DirectX::Mouse>& mouse, 
	std::unique_ptr<DirectX::Mouse::ButtonStateTracker>& tracker,
	std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker>& keysTracker) :
	m_deviceResources(deviceResources),
	m_keyboard(keyboard),
	m_mouse(mouse),
	m_tracker(tracker),
	m_keysTracker(keysTracker)
{
	m_blockManager = BlockManager();
	m_blockManager.LoadBlocks();
	m_worldGenerator = std::make_unique<WorldGenerator>(m_blockManager);

	TextureAtlas::BuildAtlas(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
	m_cam = std::make_unique<Camera>();
	m_cam->UpdateViewMatrix();
	DirectX::XMFLOAT3 playerPos = m_cam->GetPosition3f();
	m_chunksManager = std::make_unique<ChunksManager>(m_deviceResources, playerPos);
	m_view = DirectX::XMMATRIX();
	m_proj = DirectX::XMMATRIX();
	m_blockOutlineRenderer = std::make_unique<BlockOutlineRenderer>(m_deviceResources);
	auto size = m_deviceResources->GetOutputSize();
	const float aspectRatio = static_cast<float>(size.right) / static_cast<float>(size.bottom);
	m_crosshairRenderer = std::make_unique<CrosshairRenderer>(m_deviceResources, aspectRatio);
	CreateMainCB();
}

void World::Update(DX::StepTimer const& timer)
{
	auto rayCastResult = Raycast();
	//std::optional<WorldPos> rayCastResult = WorldPos(0.0f, 60.0f, 0.0f);
	auto outlinedBlock = rayCastResult.has_value() ? std::optional<WorldPos>(rayCastResult.value().first) : std::nullopt;
	m_blockOutlineRenderer->UpdateOutlinedCube(outlinedBlock);
	float elapsedTime = float(timer.GetElapsedSeconds());
	DirectX::XMFLOAT3 playerPos = m_cam->GetPosition3f();
	m_chunksManager->UpdatePlayerPos(playerPos);
	auto kb = m_keyboard->GetState();
	m_keysTracker->Update(kb);
	const float speed = 50.0f;
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
	if (m_keysTracker->IsKeyPressed(DirectX::Keyboard::F))
	{
		m_mouse->SetMode(m_mouse->GetState().positionMode == DirectX::Mouse::MODE_RELATIVE ? DirectX::Mouse::MODE_ABSOLUTE : DirectX::Mouse::MODE_RELATIVE);
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
	if (m_tracker->leftButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED && rayCastResult.has_value())
	{
		m_chunksManager->RemoveBlockAt(rayCastResult.value().first);
	}
	if (m_tracker->rightButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED && rayCastResult.has_value())
	{
		WorldPos offset = WorldUtils::GetOffsetByBlockDirection(rayCastResult.value().second);
		WorldPos placePos = rayCastResult.value().first + offset;
		m_chunksManager->PlaceBlockAt(placePos, ChunkBlock(BlockId::Cobblestone));
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
	m_chunksManager->RenderChunks();
	m_blockOutlineRenderer->RenderCubeOutline();
	m_crosshairRenderer->Render();
}

void World::OnWindowSizeChanged(float aspectRatio)
{
	m_proj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XM_PIDIV4,
		aspectRatio,
		0.1f, 1000.0f);
	m_crosshairRenderer->UpdateAspectRatio(aspectRatio);
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

std::optional<std::pair<WorldPos, ChunkBlock::BlockDirection>> World::Raycast()
{
	DirectX::XMVECTOR currentPosition = m_cam->GetPosition();
	DirectX::XMVECTOR step = DirectX::XMVectorScale(DirectX::XMVector3Normalize(m_cam->GetLook()), 0.05f);
	const float distanceStep = DirectX::XMVectorGetX(DirectX::XMVector3Length(step));
	float maxDistance = 10.0f;

	for (float distance = 0.0f; distance < maxDistance; distance += distanceStep) {
		WorldPos blockPos = WorldPos(static_cast<int>(std::floor(DirectX::XMVectorGetX(currentPosition))),
			static_cast<int>(std::floor(DirectX::XMVectorGetY(currentPosition))),
				static_cast<int>(std::floor(DirectX::XMVectorGetZ(currentPosition))));
		if (m_chunksManager->CheckBlockCollision(blockPos)) {
			DirectX::XMFLOAT3 collisionPoint;
			DirectX::XMStoreFloat3(&collisionPoint, currentPosition);
			return std::pair<WorldPos, ChunkBlock::BlockDirection>(blockPos, WorldUtils::GetNearBlockFaceToPoint(collisionPoint));
		}

		currentPosition = DirectX::XMVectorAdd(currentPosition, step);
	}

	return std::nullopt;
}