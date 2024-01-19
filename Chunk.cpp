#include "pch.h"
#include "Chunk.h"
#include "UVCoords.h"
#include "TextureAtlas.h"
#include "MathUtils.h"

Chunk::Chunk(ChunkPos& worldPos) :
    m_worldPos(worldPos)
{
    m_blocks.resize(static_cast<size_t>(SQ_WIDTH));
}

void Chunk::RemoveBlock(int x, int y, int z) noexcept
{
    assert(x < Chunk::WIDTH);
    assert(y < Chunk::HEIGHT);
    assert(z < Chunk::WIDTH);
    const size_t index = GetIdxFromCoords(x, y, z);
    if (index >= m_blocks.size())
    {
        return;
    }
    m_blocks[index] = ChunkBlock(BlockId::Air);
}

void Chunk::AddBlock(int x, int y, int z, BlockId blockId) noexcept
{
    assert(x < Chunk::WIDTH);
    assert(y < Chunk::HEIGHT);
    assert(z < Chunk::WIDTH);
    const size_t index = GetIdxFromCoords(x, y, z);
    if (index >= m_blocks.size())
    {
        const int prevY = (static_cast<int>(m_blocks.size()) / SQ_WIDTH) - 1;
        if (prevY < y)
        {
            m_blocks.resize(m_blocks.size() + static_cast<size_t>((y - prevY) * SQ_WIDTH));
        }
    }
    m_blocks[index] = ChunkBlock(blockId);
}

void Chunk::AddBlock(int x, int y, int z, ChunkBlock block) noexcept
{
    assert(x < Chunk::WIDTH);
    assert(y < Chunk::HEIGHT);
    assert(z < Chunk::WIDTH);
    const size_t index = GetIdxFromCoords(x, y, z);
    if (index >= m_blocks.size())
    {
        const int prevY = static_cast<int>(m_blocks.size()) / SQ_WIDTH;
        if (prevY < y)
        {
            m_blocks.resize(m_blocks.size() + static_cast<size_t>((y - prevY) * SQ_WIDTH));
        }
    }
    m_blocks[index] = block;
}

std::optional<ChunkBlock> Chunk::GetBlock(int x, int y, int z) noexcept
{
    assert(x < Chunk::WIDTH);
    assert(y < Chunk::HEIGHT);
    assert(z < Chunk::WIDTH);
    const size_t index = GetIdxFromCoords(x, y, z);
    if (index >= m_blocks.size())
    {
        return std::nullopt;
    }
    return m_blocks[index];
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

inline void Chunk::AddFrontFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 1.0f, 0.0f, idx }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 0.0f, 0.0f, idx }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(3 + offset);
    m_indices.emplace_back(2 + offset);
}

inline void Chunk::AddBackFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 1.0f, 1.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 0.0f, 1.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(3 + offset);
}

inline void Chunk::AddTopFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(3 + offset);
    m_indices.emplace_back(2 + offset);
}

inline void Chunk::AddBottomFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(3 + offset);
}

inline void Chunk::AddLeftFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 1.0f, 0.0f, idx }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 0.0f, 1.0f, idx }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(3 + offset);
    m_indices.emplace_back(2 + offset);
}

inline void Chunk::AddRightFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 0.0f, 0.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 1.0f, 1.0f, idx }));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(3 + offset);
}

inline size_t Chunk::GetIdxFromCoords(int x, int y, int z) const noexcept
{
    return static_cast<size_t>(x + z * WIDTH + y * SQ_WIDTH);
}

void Chunk::UpdateMeshWithoutBuffers(BlockManager& blockManager, std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks)
{
    m_vertices.clear();
    m_indices.clear();
    const int maxY = m_blocks.size() / SQ_WIDTH;
    for (int y = 0; y < maxY; y++)
    {
        for (int z = 0; z < WIDTH; z++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                const size_t idx = GetIdxFromCoords(x, y, z);
                if (idx >= m_blocks.size())
                {
                    return;
                }
                BlockId blockId = m_blocks[idx].GetId();
                if (blockId == BlockId::Air)
                {
                    continue;
                }
                WorldPos blockChunkPos = WorldPos(x, y, z);
                WorldPos pos = blockChunkPos + m_worldPos;
                WorldPos leftPos = pos - WorldPos(1, 0, 0);
                WorldPos rightPos = pos + WorldPos(1, 0, 0);
                WorldPos frontPos = pos - WorldPos(0, 0, 1);
                WorldPos backPos = pos + WorldPos(0, 0, 1);

                Block& block = blockManager.GetBlockById(blockId);
                DirectX::XMFLOAT3 blockPos = DirectX::XMFLOAT3(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));

                if (!HasBlockInWorld(frontPos, chunks))
                {
                    AddFrontFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Front));
                }
                if (!HasBlockInWorld(backPos, chunks))
                {
                    AddBackFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Back));
                }
                if (pos.y == HEIGHT - 1 || !HasBlockAt(x, y + 1, z))
                {
                    AddTopFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Top));
                }
                if (pos.y == 0 || !HasBlockAt(x, y - 1, z))
                {
                    AddBottomFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Bottom));
                }
                if (!HasBlockInWorld(leftPos, chunks))
                {
                    AddLeftFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Left));
                }
                if (!HasBlockInWorld(rightPos, chunks))
                {
                    AddRightFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Right));
                }
            }
        }
    }
}

inline bool Chunk::HasBlockInWorld(WorldPos& worldPos, std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks)
{
    int xPos = MathUtils::RoundDown(static_cast<int>(worldPos.x), WIDTH);
    int zPos = MathUtils::RoundDown(static_cast<int>(worldPos.z), WIDTH);
    ChunkPos chunkPos = ChunkPos(xPos, zPos);
    auto it = chunks.find(chunkPos);
    if (it == chunks.end())
    {
        return false;
    }
    return it->second->HasBlockAt(worldPos.x - xPos, worldPos.y, worldPos.z - zPos);
}

inline bool Chunk::HasBlockAt(int x, int y, int z) const noexcept
{
    assert(x < Chunk::WIDTH);
    //assert(chunkPos.y < Chunk::HEIGHT);
    assert(z < Chunk::WIDTH);
    const size_t idx = GetIdxFromCoords(x, y, z);
    if (idx >= m_blocks.size())
    {
        return false;
    }
    return m_blocks[idx].GetId() != BlockId::Air;
}

bool Chunk::IsModified() const noexcept
{
    return m_isModified;
}

void Chunk::SetIsModified(bool isModified) noexcept
{
    m_isModified = isModified;
}

bool Chunk::ShouldRender() const noexcept
{
    return m_shouldRender;
}

void Chunk::SetShouldRender(bool shouldRender) noexcept
{
    m_shouldRender = shouldRender;
}

ChunkPos& Chunk::GetPos() noexcept
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
    if (m_vertices.empty())
    {
        return;
    }
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
    if (m_indices.empty())
    {
        return;
    }
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
