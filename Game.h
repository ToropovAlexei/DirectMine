//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Camera.h"
#include "CubeRenderer.h"
#include "World.h"
#include "TextureAtlas.h"

struct MainConstantBuffer
{
    DirectX::XMMATRIX viewProj;
};

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
    
    void CreateMainConstantBuffer();
    void UpdateMainConstantBuffer();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_mainCB;
    DirectX::XMMATRIX m_view;
    DirectX::XMMATRIX m_proj;

    std::unique_ptr<Camera> m_cam;
    std::unique_ptr<DirectX::Keyboard> m_keyboard;
    std::unique_ptr<DirectX::Mouse> m_mouse;
    std::unique_ptr<CubeRenderer> m_cubeRenderer;

    std::unique_ptr<World> m_world;

    DirectX::Mouse::ButtonStateTracker m_tracker;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;
};
