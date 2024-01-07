#include "pch.h"
#include "Chunk.h"

Chunk::Chunk()
{

}

const std::unordered_map<Chunk::BlockPos, Cube, Chunk::BlockPosHash>& Chunk::GetBlocks() const noexcept
{
	return m_blocks;
}

void Chunk::FillWith()
{
	for (size_t x = 0; x < Chunk::WIDTH; x++)
	{
		for (size_t y = 0; y < Chunk::HEIGHT; y++)
		{
			for (size_t z = 0; z < Chunk::DEPTH; z++)
			{
				BlockPos pos = BlockPos(x, y, z);
				m_blocks.insert({ pos, Cube() });
			}
		}
	}
}

DirectX::GeometricPrimitive::VertexCollection& Chunk::GetVertices()
{
    return m_vertices;
}

DirectX::GeometricPrimitive::IndexCollection& Chunk::GetIndices()
{
    return m_indices;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Chunk::GetVertexBuffer()
{
    return m_vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Chunk::GetIndexBuffer()
{
    return m_indexBuffer;
}

void Chunk::UpdateMesh(ID3D11Device* device)
{
	for (auto& blockPair : m_blocks)
	{
        DirectX::GeometricPrimitive::VertexType vertex1 = DirectX::GeometricPrimitive::VertexType(DirectX::XMFLOAT3(-0.5f + blockPair.first.x, -0.5f + blockPair.first.y, 0.5f + blockPair.first.z), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(0.0f, 0.0f));
        DirectX::GeometricPrimitive::VertexType vertex2 = DirectX::GeometricPrimitive::VertexType(DirectX::XMFLOAT3(0.5f + blockPair.first.x, -0.5f + blockPair.first.y, 0.5f + blockPair.first.z), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1.0f, 0.0f));
        DirectX::GeometricPrimitive::VertexType vertex3 = DirectX::GeometricPrimitive::VertexType(DirectX::XMFLOAT3(0.5f + blockPair.first.x, 0.5f + blockPair.first.y, 0.5f + blockPair.first.z), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1.0f, 1.0f));
        DirectX::GeometricPrimitive::VertexType vertex4 = DirectX::GeometricPrimitive::VertexType(DirectX::XMFLOAT3(-0.5f + blockPair.first.x, 0.5f + blockPair.first.y, 0.5f + blockPair.first.z), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(0.0f, 1.0f));
        m_vertices.push_back(vertex1);
        m_vertices.push_back(vertex2);
        m_vertices.push_back(vertex3);
        m_vertices.push_back(vertex4);
        int offset = m_vertices.size();
        m_indices.push_back(2 + offset);
        m_indices.push_back(1 + offset);
        m_indices.push_back(0 + offset);
        m_indices.push_back(3 + offset);
        m_indices.push_back(2 + offset);
        m_indices.push_back(0 + offset);
	}
    BuildVertexBuffer(device);
    BuildIndexBuffer(device);
}

void Chunk::BuildVertexBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = static_cast<UINT>(sizeof(DirectX::GeometricPrimitive::VertexType) * m_vertices.size());
    bd.StructureByteStride = sizeof(DirectX::GeometricPrimitive::VertexType);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = m_vertices.data();
    device->CreateBuffer(&bd, &sd, &m_vertexBuffer);
}

void Chunk::BuildIndexBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = static_cast<UINT>(sizeof(uint16_t) * m_indices.size());
    ibd.StructureByteStride = sizeof(uint16_t);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = m_indices.data();
    device->CreateBuffer(&ibd, &isd, &m_indexBuffer);
}
