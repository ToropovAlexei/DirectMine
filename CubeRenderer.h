#pragma once

#include "Cube.h"
#include "Chunk.h"
#include "TextureAtlas.h"

class CubeRenderer
{
public:
	CubeRenderer(ID3D11Device* device);

    void DrawChunk(ID3D11DeviceContext1* context, Chunk* chunk);

private:
	void BuildInputLayout(ID3D11Device* device);
    void BuildSampler(ID3D11Device* device);
    void BuildBlendState(ID3D11Device* device);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;

    UINT m_stride = sizeof(Vertex);
    UINT m_offset = 0u;
};

