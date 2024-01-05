#pragma once

#include "Cube.h"
#include "TexturesManager.h"

class CubeRenderer
{
public:
	CubeRenderer(ID3D11Device* device);

    void DrawCubes(ID3D11DeviceContext1* context, std::vector<std::unique_ptr<Cube>>& cubes);

private:
	void BuildInputLayout(ID3D11Device* device);
    void BuildVerticesAndIndices();
    void BuildVertexBuffer(ID3D11Device* device);
    void BuildIndexBuffer(ID3D11Device* device);
    void BuildSampler(ID3D11Device* device);
    void BuildConstantBuffer(ID3D11Device* device);

    void UpdateWorldConstantBuffer(ID3D11DeviceContext1* context);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

    DirectX::GeometricPrimitive::VertexCollection vertices;
    DirectX::GeometricPrimitive::IndexCollection indices;

    std::unique_ptr<TexturesManager> m_texManager;

    UINT m_stride = sizeof(DirectX::GeometricPrimitive::VertexType);
    UINT m_offset = 0u;
    UINT m_cbSize = sizeof(DirectX::XMMATRIX);

    DirectX::XMMATRIX m_activeCB = DirectX::XMMATRIX();
};

