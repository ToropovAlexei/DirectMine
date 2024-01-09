#pragma once

#include "Cube.h"
#include "Chunk.h"
#include "TextureAtlas.h"
#include "DeviceResources.h"

class ChunkRenderer
{
public:
	ChunkRenderer(std::unique_ptr<DX::DeviceResources>& deviceResources);

    void DrawChunk(Chunk* chunk);
    void RenderChunks(std::vector<std::unique_ptr<Chunk>>& chunks);

private:
	void BuildInputLayout();
    void BuildSampler();
    void BuildBlendState();

private:
    std::unique_ptr<DX::DeviceResources>& m_deviceResources;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;

    UINT m_stride = sizeof(Vertex);
    UINT m_offset = 0u;
};

