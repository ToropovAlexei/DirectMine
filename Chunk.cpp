#include "pch.h"
#include "Chunk.h"
#include "UVCoords.h"
#include "TextureAtlas.h"

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
                if (rand() % 10 > 1)
                {
                    Chunk::BlockPos pos = Chunk::BlockPos(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
                    m_blocks.insert({ pos, Cube() });
                }
			}
		}
	}
}

std::vector<Vertex>& Chunk::GetVertices()
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

void Chunk::AddFrontFace(DirectX::XMFLOAT3 pos, std::string texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());
    std::vector<Vertex> vertices = {
        {{0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z}, {uvCoords.u1, uvCoords.v2}},   // ������� 0
        {{1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z}, {uvCoords.u2, uvCoords.v2}},   // ������� 1
        {{1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z}, {uvCoords.u2, uvCoords.v1}},   // ������� 2
        {{0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z}, {uvCoords.u1, uvCoords.v1}}    // ������� 3
    };

    std::vector<UINT> indices = {
        0 + offset, 2 + offset, 1 + offset,    // ������ ����������� (�������� �����)
        0 + offset, 3 + offset, 2 + offset,    // ������ ����������� (�������� �����)
    };
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
    m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}

void Chunk::AddBackFace(DirectX::XMFLOAT3 pos, std::string texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());
    std::vector<Vertex> vertices = {
        {{0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z}, {uvCoords.u2, uvCoords.v2}},   // ������� 0
        {{1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z}, {uvCoords.u1, uvCoords.v2}},   // ������� 1
        {{1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z}, {uvCoords.u1, uvCoords.v1}},   // ������� 2
        {{0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z}, {uvCoords.u2, uvCoords.v1}}    // ������� 3
    };

    std::vector<UINT> indices = {
        0 + offset, 1 + offset, 2 + offset,    // ������ ����������� (�������� �����)
        0 + offset, 2 + offset, 3 + offset,    // ������ ����������� (�������� �����)
    };
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
    m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}

void Chunk::AddTopFace(DirectX::XMFLOAT3 pos, std::string texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());
    std::vector<Vertex> vertices = {
        {{0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z}, {uvCoords.u1, uvCoords.v2}},   // ������� 0
        {{1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z}, {uvCoords.u2, uvCoords.v2}},   // ������� 1
        {{1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z}, {uvCoords.u2, uvCoords.v1}},   // ������� 2
        {{0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z}, {uvCoords.u1, uvCoords.v1}}    // ������� 3
    };

    std::vector<UINT> indices = {
        0 + offset, 2 + offset, 1 + offset,    // ������ ����������� (�������� �����)
        0 + offset, 3 + offset, 2 + offset,    // ������ ����������� (�������� �����)
    };
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
    m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}

void Chunk::AddBottomFace(DirectX::XMFLOAT3 pos, std::string texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());
    std::vector<Vertex> vertices = {
        {{0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z}, {uvCoords.u2, uvCoords.v2}},   // ������� 0
        {{1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z}, {uvCoords.u1, uvCoords.v2}},   // ������� 1
        {{1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z}, {uvCoords.u1, uvCoords.v1}},   // ������� 2
        {{0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z}, {uvCoords.u2, uvCoords.v1}}    // ������� 3
    };

    std::vector<UINT> indices = {
        0 + offset, 1 + offset, 2 + offset,    // ������ ����������� (�������� �����)
        0 + offset, 2 + offset, 3 + offset,    // ������ ����������� (�������� �����)
    };
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
    m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}

void Chunk::AddLeftFace(DirectX::XMFLOAT3 pos, std::string texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());
    std::vector<Vertex> vertices = {
        {{0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z}, {uvCoords.u2, uvCoords.v2}},   // ������� 0
        {{0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z}, {uvCoords.u2, uvCoords.v1}},   // ������� 1
        {{0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z}, {uvCoords.u1, uvCoords.v1}},   // ������� 2
        {{0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z}, {uvCoords.u1, uvCoords.v2}}    // ������� 3
    };

    std::vector<UINT> indices = {
        0 + offset, 2 + offset, 1 + offset,    // ������ ����������� (�������� �����)
        0 + offset, 3 + offset, 2 + offset,    // ������ ����������� (�������� �����)
    };
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
    m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}

void Chunk::AddRightFace(DirectX::XMFLOAT3 pos, std::string texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());
    std::vector<Vertex> vertices = {
        {{1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z}, {uvCoords.u1, uvCoords.v2}},   // ������� 0
        {{1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z}, {uvCoords.u1, uvCoords.v1}},   // ������� 1
        {{1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z}, {uvCoords.u2, uvCoords.v1}},   // ������� 2
        {{1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z}, {uvCoords.u2, uvCoords.v2}}    // ������� 3
    };

    std::vector<UINT> indices = {
        0 + offset, 1 + offset, 2 + offset,    // ������ ����������� (�������� �����)
        0 + offset, 2 + offset, 3 + offset,    // ������ ����������� (�������� �����)
    };
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
    m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}

void Chunk::UpdateMesh(ID3D11Device* device)
{
    for (auto& blockPair : m_blocks)
    {
        Chunk::BlockPos pos = blockPair.first;
        Chunk::BlockPos topPos = blockPair.first + Chunk::BlockPos(0.0f, 1.0f, 0.0f);
        Chunk::BlockPos bottomPos = blockPair.first - Chunk::BlockPos(0.0f, 1.0f, 0.0f);
        Chunk::BlockPos leftPos = blockPair.first - Chunk::BlockPos(1.0f, 0.0f, 0.0f);
        Chunk::BlockPos rightPos = blockPair.first + Chunk::BlockPos(1.0f, 0.0f, 0.0f);
        Chunk::BlockPos frontPos = blockPair.first - Chunk::BlockPos(0.0f, 0.0f, 1.0f);
        Chunk::BlockPos backPos = blockPair.first + Chunk::BlockPos(0.0f, 0.0f, 1.0f);
        if (pos.z == 0 || !HasBlockAt(frontPos))
        {
            AddFrontFace(DirectX::XMFLOAT3(pos.x, pos.y, pos.z), "grass_side");
        }
        if (pos.z == DEPTH - 1 || !HasBlockAt(backPos))
        {
            AddBackFace(DirectX::XMFLOAT3(pos.x, pos.y, pos.z), "grass_side");
        }
        if (pos.y == HEIGHT - 1 || !HasBlockAt(topPos))
        {
            AddTopFace(DirectX::XMFLOAT3(pos.x, pos.y, pos.z), "grass_top");
        }
        if (pos.y == 0 || !HasBlockAt(bottomPos))
        {
            AddBottomFace(DirectX::XMFLOAT3(pos.x, pos.y, pos.z), "dirt");
        }
        if (pos.x == 0 || !HasBlockAt(leftPos))
        {
            AddLeftFace(DirectX::XMFLOAT3(pos.x, pos.y, pos.z), "grass_side");
        }
        if (pos.x == WIDTH - 1 || !HasBlockAt(rightPos))
        {
            AddRightFace(DirectX::XMFLOAT3(pos.x, pos.y, pos.z), "grass_side");
        }
    }

    BuildVertexBuffer(device);
    BuildIndexBuffer(device);
}

bool Chunk::HasBlockAt(BlockPos& pos)
{
    return m_blocks.contains(pos);
}

void Chunk::BuildVertexBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * m_vertices.size());
    bd.StructureByteStride = sizeof(Vertex);
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
