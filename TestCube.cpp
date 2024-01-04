#include "pch.h"
#include "TestCube.h"

TestCube::TestCube(ID3D11Device* device)
{
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
    vertices =
    {
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
        { { -0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
    };

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(vertices.size());
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData = {};
    vertexBufferData.pSysMem = &vertices;

    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);

    if (result != S_OK)
    {
        throw std::runtime_error("Failed to create buffer");
        return;
    }
}

void TestCube::BuildIndexBuffer(ID3D11Device* device)
{
    // ����������� �������� ��� ����
    indices =
    {
        // ������ �������
        0, 1, 2,
        2, 3, 0,

        // ������ �������
        1, 5, 6,
        6, 2, 1,

        // ������ �������
        4, 7, 6,
        6, 5, 4,

        // ��������� �������
        0, 3, 7,
        7, 4, 0,

        // ����� �������
        3, 2, 6,
        6, 7, 3,

        // ������ �������
        0, 4, 5,
        5, 1, 0
    };

    // �������� ������ ��������
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(UINT) * static_cast<UINT>(indices.size());
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    // ������ ��� ������ ��������
    D3D11_SUBRESOURCE_DATA indexBufferData = {};
    indexBufferData.pSysMem = &indices;

    // �������� ���������� ������
    HRESULT result = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);

    if (result != S_OK)
    {
        throw std::runtime_error("Failed to create buffer");
        return;
    }
}
