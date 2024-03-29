#pragma once

#include "Chunk.h"
#include "TextureAtlas.h"
#include "DeviceResources.h"
#include "WorldPos.hpp"

class ChunkRenderer
{
public:
	ChunkRenderer(std::unique_ptr<DX::DeviceResources>& deviceResources);

    void RenderChunks(std::vector<std::shared_ptr<Chunk>>& chunks);

private:
	void BuildInputLayout();
    void BuildSampler();
    void BuildBlendState();
    void BuildRasterState();

private:
    std::unique_ptr<DX::DeviceResources>& m_deviceResources;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterState;

    UINT m_stride = sizeof(Vertex);
    UINT m_offset = 0u;
};

