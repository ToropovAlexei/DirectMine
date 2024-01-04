#include "pch.h"
#include "TestCube.h"

TestCube::TestCube(ID3D11Device* device)
{
    DirectX::GeometricPrimitive::CreateCube(vertices, indices, 1.0f, false);

    // Back
    vertices[0].textureCoordinate = { 0.75f, 1.0f / 3.0f };
    vertices[1].textureCoordinate = { 0.75f, 2.0f / 3.0f };
    vertices[2].textureCoordinate = { 1.0f, 2.0f / 3.0f };
    vertices[3].textureCoordinate = { 1.0f, 1.0f / 3.0f };
    // Front
    vertices[4].textureCoordinate = { 0.25f, 1.0f / 3.0f };
    vertices[5].textureCoordinate = { 0.25f, 2.0f / 3.0f };
    vertices[6].textureCoordinate = { 0.5f, 2.0f / 3.0f };
    vertices[7].textureCoordinate = { 0.5f, 1.0f / 3.0f };
    // Right
    vertices[8].textureCoordinate = { 0.5f, 1.0f / 3.0f };
    vertices[9].textureCoordinate = { 0.5f, 2.0f / 3.0f };
    vertices[10].textureCoordinate = { 0.75f, 2.0f / 3.0f };
    vertices[11].textureCoordinate = { 0.75f, 1.0f / 3.0f };
    // Left
    vertices[12].textureCoordinate = { 0.0f, 1.0f / 3.0f };
    vertices[13].textureCoordinate = { 0.0f, 2.0f / 3.0f };
    vertices[14].textureCoordinate = { 0.25f, 2.0f / 3.0f };
    vertices[15].textureCoordinate = { 0.25f, 1.0f / 3.0f };
    // Top
    vertices[16].textureCoordinate = { 0.25f, 0.0f };
    vertices[17].textureCoordinate = { 0.25f, 1.0f / 3.0f };
    vertices[18].textureCoordinate = { 0.5f, 1.0f / 3.0f };
    vertices[19].textureCoordinate = { 0.5f, 0.0f };
    // Bottom
    vertices[20].textureCoordinate = { 0.25f, 2.0f / 3.0f };
    vertices[21].textureCoordinate = { 0.25f, 1.0f };
    vertices[22].textureCoordinate = { 0.5f, 1.0f };
    vertices[23].textureCoordinate = { 0.5f, 2.0f / 3.0f };

    BuildVertexBuffer(device);
    BuildIndexBuffer(device);
}

Microsoft::WRL::ComPtr<ID3D11Buffer> TestCube::VertexBuffer()
{
    return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> TestCube::IndexBuffer()
{
    return indexBuffer;
}

void TestCube::BuildVertexBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = sizeof(DirectX::GeometricPrimitive::VertexType) * vertices.size();
    bd.StructureByteStride = sizeof(DirectX::GeometricPrimitive::VertexType);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices.data();
    device->CreateBuffer(&bd, &sd, &vertexBuffer);
}

void TestCube::BuildIndexBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = sizeof(uint16_t) * indices.size();
    ibd.StructureByteStride = sizeof(uint16_t);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices.data();
    device->CreateBuffer(&ibd, &isd, &indexBuffer);
}
