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
    Vertex vertices[] =
    {
        { -1.0f,-1.0f,-1.0f	 },
        { 1.0f,-1.0f,-1.0f	 },
        { -1.0f,1.0f,-1.0f	 },
        { 1.0f,1.0f,-1.0f	  },
        { -1.0f,-1.0f,1.0f	 },
        { 1.0f,-1.0f,1.0f	  },
        { -1.0f,1.0f,1.0f	 },
        { 1.0f,1.0f,1.0f	 },
    };
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;
    device->CreateBuffer(&bd, &sd, &vertexBuffer);
}

void TestCube::BuildIndexBuffer(ID3D11Device* device)
{
    const unsigned short indices[] =
    {
        0,2,1, 2,3,1,
        1,3,5, 3,7,5,
        2,6,3, 3,6,7,
        4,5,7, 4,7,6,
        0,4,2, 2,4,6,
        0,1,4, 1,5,4
    };
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(unsigned short);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    device->CreateBuffer(&ibd, &isd, &indexBuffer);
}
