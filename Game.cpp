//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "ShadersLoader.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
	m_cam = std::make_unique<Camera>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_cam->UpdateViewMatrix();
    m_world = std::make_unique<World>(m_cam.get());
    auto& chunks = m_world->Chunks();
    for (auto& chunk : chunks)
    {
        chunk->FillWith();
    }
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);
    m_mouse->SetWindow(window);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
    Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
    if (FAILED(initialize))
    {
        throw std::runtime_error("Failed to initialize WIC");
    }
#else
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to initialize WIC");
    }
#endif

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    std::wstring title = L"DirectX 11 FPS: "
        + std::to_wstring(timer.GetFramesPerSecond())
        + L" x:" + std::to_wstring(static_cast<int>(m_cam->GetPosition3f().x))
        + L" y:" + std::to_wstring(static_cast<int>(m_cam->GetPosition3f().y))
        + L" z:" + std::to_wstring(static_cast<int>(m_cam->GetPosition3f().z));
    SetWindowText(GetActiveWindow(), title.c_str());
	UpdateMainConstantBuffer();

    auto kb = m_keyboard->GetState();
    if (kb.Escape)
    {
        ExitGame();
    }

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
    m_tracker.Update(mouse);

    if (mouse.positionMode == Mouse::MODE_RELATIVE)
    {
        float dx = XMConvertToRadians(0.05f * static_cast<float>(mouse.x));
        float dy = XMConvertToRadians(0.05f * static_cast<float>(mouse.y));
        m_cam->Pitch(dy);
        m_cam->RotateY(dx);
    }

    if (m_tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::PRESSED)
    {
        m_mouse->SetMode(Mouse::MODE_RELATIVE);
    }
    else if (m_tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
    {
        m_mouse->SetMode(Mouse::MODE_ABSOLUTE);
    }

    m_cam->UpdateViewMatrix();
    m_view = m_cam->GetView();
    UpdateMainConstantBuffer();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    context->VSSetConstantBuffers(0u, 1u, m_mainCB.GetAddressOf());
    auto& chunks = m_world->Chunks();
    for (auto& chunk : chunks)
    {
        m_cubeRenderer->DrawChunk(context, chunk.get());
    }

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
	CreateMainConstantBuffer();
    m_cubeRenderer = std::make_unique<CubeRenderer>(device);
    TextureAtlas::BuildAtlas(device, m_deviceResources->GetD3DDeviceContext());
    m_world->UpdateMesh(device);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
	auto size = m_deviceResources->GetOutputSize();
	const float aspectRatio = static_cast<float>(size.right) / static_cast<float>(size.bottom);

	m_proj = DirectX::XMMatrixPerspectiveFovLH(
		XM_PIDIV4,
		aspectRatio,
		0.1f, 250.0f);
}

void Game::CreateMainConstantBuffer()
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

void Game::UpdateMainConstantBuffer()
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

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
