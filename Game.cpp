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
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

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
    elapsedTime;
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

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	context->IASetVertexBuffers(0u, 1u, m_cube->VertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_cube->IndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0u);

	// create constant buffer for transformation matrix
	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};
	const ConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationZ(45.0f) *
				DirectX::XMMatrixRotationX(45.0f) *
				DirectX::XMMatrixTranslation(0.0f,0.0f,4.0f)
			)
		}
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	m_deviceResources->GetD3DDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer);

	// bind constant buffer to vertex shader
	context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	const ConstantBuffer vcb2 =
	{
		{
			DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveLH(1.0f,3.0f / 4.0f,0.5f,1000.0f))
		}
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pvConstantBuffer2;
	D3D11_BUFFER_DESC cbdv2;
	cbdv2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbdv2.Usage = D3D11_USAGE_DYNAMIC;
	cbdv2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbdv2.MiscFlags = 0u;
	cbdv2.ByteWidth = sizeof(vcb2);
	cbdv2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csdv2 = {};
	csdv2.pSysMem = &vcb2;
	m_deviceResources->GetD3DDevice()->CreateBuffer(&cbdv2, &csdv2, &pvConstantBuffer2);

	// bind constant buffer to vertex shader
	context->VSSetConstantBuffers(1u, 1u, pvConstantBuffer2.GetAddressOf());


	// lookup table for cube face colors
	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};
	const ConstantBuffer2 cb2 =
	{
		{
			{1.0f,0.0f,1.0f},
			{1.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f},
			{1.0f,1.0f,0.0f},
			{0.0f,1.0f,1.0f},
		}
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer2;
	D3D11_BUFFER_DESC cbd2;
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DEFAULT;
	cbd2.CPUAccessFlags = 0u;
	cbd2.MiscFlags = 0u;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd2 = {};
	csd2.pSysMem = &cb2;
	m_deviceResources->GetD3DDevice()->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2);

	context->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
	context->IASetInputLayout(m_inputLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(36u, 0u, 0u);

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
    m_vertexShader = ShadersLoader::LoadVertexShader(device, L"VertexShader.cso");
    m_pixelShader = ShadersLoader::LoadPixelShader(device, L"PixelShader.cso");
    CreateInputLayout();
    m_cube = std::make_unique<TestCube>(device);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Game::CreateInputLayout()
{
    auto device = m_deviceResources->GetD3DDevice();
    // Описание элементов вершинного формата
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    // Создание input layout
    auto vertexShaderBlob = ShadersLoader::LoadBlob(device, L"VertexShader.cso");
    device->CreateInputLayout(inputLayoutDesc,
        ARRAYSIZE(inputLayoutDesc),
        vertexShaderBlob->GetBufferPointer(), 
        vertexShaderBlob->GetBufferSize(),
        &m_inputLayout);
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
