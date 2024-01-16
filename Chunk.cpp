#include "pch.h"
#include "Chunk.h"
#include "UVCoords.h"
#include "TextureAtlas.h"
#include "MathUtils.h"

Chunk::Chunk(ChunkPos& worldPos) :
    m_worldPos(worldPos)
{
}

const std::unordered_map<WorldPos, ChunkBlock, WorldPosHash>& Chunk::GetBlocks() const noexcept
{
	return m_blocks;
}

void Chunk::RemoveBlock(WorldPos& worldPos) noexcept
{
    assert(worldPos.x < Chunk::WIDTH);
    assert(worldPos.y < Chunk::HEIGHT);
    assert(worldPos.z < Chunk::DEPTH);
    auto it = m_blocks.find(worldPos);
    if (it == m_blocks.end())
    {
        return;
    }
    m_blocks.erase(it);
}

void Chunk::AddBlock(WorldPos& worldPos, BlockId blockId) noexcept
{
    assert(worldPos.x < Chunk::WIDTH);
    assert(worldPos.y < Chunk::HEIGHT);
    assert(worldPos.z < Chunk::DEPTH);
    m_blocks.insert({ worldPos, ChunkBlock(blockId) });
}

void Chunk::AddBlock(WorldPos& worldPos, ChunkBlock block) noexcept
{
    assert(worldPos.x < Chunk::WIDTH);
    assert(worldPos.y < Chunk::HEIGHT);
    assert(worldPos.z < Chunk::DEPTH);
    m_blocks.insert({ worldPos, block });
}

std::optional<ChunkBlock> Chunk::GetBlock(WorldPos& worldPos) noexcept
{
    assert(worldPos.x < Chunk::WIDTH);
    assert(worldPos.y < Chunk::HEIGHT);
    assert(worldPos.z < Chunk::DEPTH);
    auto it = m_blocks.find(worldPos);
    if (it == m_blocks.end())
    {
        return std::nullopt;
    }
    return it->second;
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

void Chunk::UpdateMeshWithoutBuffers(BlockManager& blockManager, std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks)
{
    m_vertices.clear();
    m_indices.clear();
    for (auto& blockPair : m_blocks)
    {
        WorldPos pos = blockPair.first + m_worldPos;
        WorldPos topPos = blockPair.first + WorldPos(0, 1, 0);
        WorldPos bottomPos = blockPair.first - WorldPos(0, 1, 0);
        WorldPos leftPos = pos - WorldPos(1, 0, 0);
        WorldPos rightPos = pos + WorldPos(1, 0, 0);
        WorldPos frontPos = pos - WorldPos(0, 0, 1);
        WorldPos backPos = pos + WorldPos(0, 0, 1);

        BlockId blockId = blockPair.second.GetId();
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
        if (pos.y == HEIGHT - 1 || !HasBlockAt(topPos))
        {
            AddTopFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Top));
        }
        if (pos.y == 0 || !HasBlockAt(bottomPos))
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

inline bool Chunk::HasBlockInWorld(WorldPos& worldPos, std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks)
{
    int xPos = MathUtils::RoundDown(static_cast<int>(worldPos.x), Chunk::WIDTH);
    int zPos = MathUtils::RoundDown(static_cast<int>(worldPos.z), Chunk::DEPTH);
    ChunkPos chunkPos = ChunkPos(xPos, zPos);
    auto it = chunks.find(chunkPos);
    if (it == chunks.end())
    {
        return false;
    }
    WorldPos blockPos = WorldPos(worldPos.x - xPos, worldPos.y, worldPos.z - zPos);
    return it->second->HasBlockAt(blockPos);
}

inline bool Chunk::HasBlockAt(WorldPos& chunkPos)  const noexcept
{
    assert(chunkPos.x < Chunk::WIDTH);
    //assert(chunkPos.y < Chunk::HEIGHT);
    assert(chunkPos.z < Chunk::DEPTH);
    return m_blocks.contains(chunkPos);
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
