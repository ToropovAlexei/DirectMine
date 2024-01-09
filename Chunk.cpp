#include "pch.h"
#include "Chunk.h"
#include "UVCoords.h"
#include "TextureAtlas.h"

Chunk::Chunk(WorldPos& worldPos) :
    m_worldPos(worldPos)
{
}

const std::unordered_map<WorldPos, ChunkBlock, WorldPosHash>& Chunk::GetBlocks() const noexcept
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
                WorldPos pos = WorldPos(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
                m_blocks.insert({ pos, ChunkBlock(rand() % 12) });
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

inline void Chunk::AddFrontFace(DirectX::XMFLOAT3& pos, std::string& texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, {uvCoords.u1, uvCoords.v2}));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { uvCoords.u2, uvCoords.v2 }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { uvCoords.u2, uvCoords.v1 }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { uvCoords.u1, uvCoords.v1 }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(3 + offset);
    m_indices.emplace_back(2 + offset);
}

inline void Chunk::AddBackFace(DirectX::XMFLOAT3& pos, std::string& texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { uvCoords.u2, uvCoords.v2 }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { uvCoords.u1, uvCoords.v2 }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { uvCoords.u1, uvCoords.v1 }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { uvCoords.u2, uvCoords.v1 }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(3 + offset);
}

inline void Chunk::AddTopFace(DirectX::XMFLOAT3& pos, std::string& texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { uvCoords.u1, uvCoords.v2 }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { uvCoords.u2, uvCoords.v2 }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { uvCoords.u2, uvCoords.v1 }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { uvCoords.u1, uvCoords.v1 }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(3 + offset);
    m_indices.emplace_back(2 + offset);
}

inline void Chunk::AddBottomFace(DirectX::XMFLOAT3& pos, std::string& texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { uvCoords.u2, uvCoords.v2 }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { uvCoords.u1, uvCoords.v2 }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { uvCoords.u1, uvCoords.v1 }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { uvCoords.u2, uvCoords.v1 }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(3 + offset);
}

inline void Chunk::AddLeftFace(DirectX::XMFLOAT3& pos, std::string& texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { uvCoords.u2, uvCoords.v2 }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { uvCoords.u2, uvCoords.v1 }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { uvCoords.u1, uvCoords.v1 }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { uvCoords.u1, uvCoords.v2 }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(3 + offset);
    m_indices.emplace_back(2 + offset);
}

inline void Chunk::AddRightFace(DirectX::XMFLOAT3& pos, std::string& texture)
{
    UVCoords uvCoords = TextureAtlas::GetTextureCoords(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { uvCoords.u1, uvCoords.v2 }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { uvCoords.u1, uvCoords.v1 }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { uvCoords.u2, uvCoords.v1 }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { uvCoords.u2, uvCoords.v2 }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(3 + offset);
}

void Chunk::UpdateMesh(ID3D11Device* device, BlockManager& blockManager)
{
    for (auto& blockPair : m_blocks)
    {
        WorldPos pos = m_worldPos + blockPair.first;
        WorldPos topPos = blockPair.first + WorldPos(0.0f, 1.0f, 0.0f);
        WorldPos bottomPos = blockPair.first - WorldPos(0.0f, 1.0f, 0.0f);
        WorldPos leftPos = blockPair.first - WorldPos(1.0f, 0.0f, 0.0f);
        WorldPos rightPos = blockPair.first + WorldPos(1.0f, 0.0f, 0.0f);
        WorldPos frontPos = blockPair.first - WorldPos(0.0f, 0.0f, 1.0f);
        WorldPos backPos = blockPair.first + WorldPos(0.0f, 0.0f, 1.0f);

        uint16_t blockId = blockPair.second.GetId();
        Block block = blockManager.GetBlockById(blockId);
        DirectX::XMFLOAT3 blockPos = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);

        if (pos.z == 0 || !HasBlockAt(frontPos))
        {
            AddFrontFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Front));
        }
        if (pos.z == DEPTH - 1 || !HasBlockAt(backPos))
        {
            AddBackFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Back));
        }
        if (pos.y == HEIGHT - 1 || !HasBlockAt(topPos))
        {
            AddTopFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Top));
        }
        if (pos.y == 0 || !HasBlockAt(bottomPos))
        {
            AddBottomFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Bottom));
        }
        if (pos.x == 0 || !HasBlockAt(leftPos))
        {
            AddLeftFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Left));
        }
        if (pos.x == WIDTH - 1 || !HasBlockAt(rightPos))
        {
            AddRightFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Right));
        }
    }

    BuildVertexBuffer(device);
    BuildIndexBuffer(device);
}

void Chunk::UpdateMeshWithoutBuffers(BlockManager blockManager)
{
    for (auto& blockPair : m_blocks)
    {
        WorldPos pos = m_worldPos + blockPair.first;
        WorldPos topPos = blockPair.first + WorldPos(0.0f, 1.0f, 0.0f);
        WorldPos bottomPos = blockPair.first - WorldPos(0.0f, 1.0f, 0.0f);
        WorldPos leftPos = blockPair.first - WorldPos(1.0f, 0.0f, 0.0f);
        WorldPos rightPos = blockPair.first + WorldPos(1.0f, 0.0f, 0.0f);
        WorldPos frontPos = blockPair.first - WorldPos(0.0f, 0.0f, 1.0f);
        WorldPos backPos = blockPair.first + WorldPos(0.0f, 0.0f, 1.0f);

        uint16_t blockId = blockPair.second.GetId();
        Block block = blockManager.GetBlockById(blockId);
        DirectX::XMFLOAT3 blockPos = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);

        if (pos.z == 0 || !HasBlockAt(frontPos))
        {
            AddFrontFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Front));
        }
        if (pos.z == DEPTH - 1 || !HasBlockAt(backPos))
        {
            AddBackFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Back));
        }
        if (pos.y == HEIGHT - 1 || !HasBlockAt(topPos))
        {
            AddTopFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Top));
        }
        if (pos.y == 0 || !HasBlockAt(bottomPos))
        {
            AddBottomFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Bottom));
        }
        if (pos.x == 0 || !HasBlockAt(leftPos))
        {
            AddLeftFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Left));
        }
        if (pos.x == WIDTH - 1 || !HasBlockAt(rightPos))
        {
            AddRightFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Right));
        }
    }
}

inline bool Chunk::HasBlockAt(WorldPos& pos)
{
    return m_blocks.contains(pos);
}

WorldPos& Chunk::GetPos() noexcept
{
    return m_worldPos;
}

void Chunk::UpdateBuffers(ID3D11Device* device)
{
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
