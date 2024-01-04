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
    // Определение индексов для куба
    indices =
    {
        // Первая сторона
        0, 1, 2,
        2, 3, 0,

        // Вторая сторона
        1, 5, 6,
        6, 2, 1,

        // Третья сторона
        4, 7, 6,
        6, 5, 4,

        // Четвертая сторона
        0, 3, 7,
        7, 4, 0,

        // Пятая сторона
        3, 2, 6,
        6, 7, 3,

        // Шестая сторона
        0, 4, 5,
        5, 1, 0
    };

    // Описание буфера индексов
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(UINT) * static_cast<UINT>(indices.size());
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    // Данные для буфера индексов
    D3D11_SUBRESOURCE_DATA indexBufferData = {};
    indexBufferData.pSysMem = &indices;

    // Создание индексного буфера
    HRESULT result = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);

    if (result != S_OK)
    {
        throw std::runtime_error("Failed to create buffer");
        return;
    }
}
