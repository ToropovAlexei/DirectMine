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

std::vector<UINT>& Chunk::GetIndices()
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
        Chunk::BlockPos pos = blockPair.first;
        if (pos.x != 0 && pos.y != 0 && pos.z != 0 && pos.x != WIDTH - 1 && pos.y != HEIGHT - 1 && pos.z != DEPTH - 1)
        {
            continue;
        }
        DirectX::GeometricPrimitive::VertexCollection vertices;
        DirectX::GeometricPrimitive::IndexCollection indices;

        DirectX::GeometricPrimitive::CreateCube(vertices, indices, 1.0f, false);

        int indicesOffset = m_vertices.size();

        for (auto& vertex : vertices)
        {
            vertex.position.x += blockPair.first.x;
            vertex.position.y += blockPair.first.y;
            vertex.position.z += blockPair.first.z;
        }

        for (size_t i = 0; i < indices.size(); i++)
        {
            m_indices.push_back(static_cast<UINT>(indices[i]) + indicesOffset);
        }

        m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
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
    ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_indices.size());
    ibd.StructureByteStride = sizeof(UINT);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = m_indices.data();
    device->CreateBuffer(&ibd, &isd, &m_indexBuffer);
}
