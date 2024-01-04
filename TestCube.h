#pragma once

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 texCoord;
};

class TestCube
{
public:
    TestCube(ID3D11Device* device);

    Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer();

    Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer();

private:
    void BuildVertexBuffer(ID3D11Device* device);
    void BuildIndexBuffer(ID3D11Device* device);

private:
    DirectX::GeometricPrimitive::VertexCollection vertices;
    DirectX::GeometricPrimitive::IndexCollection indices;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
};

