#include "pch.h"
#include "Chunk.h"
#include "UVCoords.h"
#include "TextureAtlas.h"
#include "MathUtils.h"

Chunk::Chunk(ChunkPos& worldPos) :
    m_worldPos(worldPos)
{
    m_blocks.resize(static_cast<size_t>(SQ_WIDTH));
    m_lightMap.resize(static_cast<size_t>(SQ_WIDTH));
}

void Chunk::SetBlock(int x, int y, int z, BlockId blockId) noexcept
{
    SetBlock(x, y, z, ChunkBlock(blockId));
}

void Chunk::SetBlock(int x, int y, int z, ChunkBlock block) noexcept
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
            const size_t newSize = m_blocks.size() + static_cast<size_t>((y - prevY) * SQ_WIDTH);
            m_blocks.resize(newSize);
            // ����� ������������ ������, �.�. � lightMap ����� ���� ������ ���������
            m_lightMap.resize(std::max(newSize, m_lightMap.size()));
        }
    }
    m_blocks[index] = block;
    if (block.GetId() == BlockId::Air)
    {
        ShrinkAirBlocks();
    }
}

ChunkBlock Chunk::GetBlock(int x, int y, int z) noexcept
{
    assert(x < Chunk::WIDTH);
    assert(y < Chunk::HEIGHT);
    assert(z < Chunk::WIDTH);
    const size_t index = GetIdxFromCoords(x, y, z);
    if (index >= m_blocks.size())
    {
        return ChunkBlock();
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

inline void Chunk::AddFrontFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 1.0f, 0.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 0.0f, 0.0f, idx }, light));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(3 + offset);
    m_indices.emplace_back(2 + offset);
}

inline void Chunk::AddBackFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 1.0f, 1.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 0.0f, 1.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }, light));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(3 + offset);
}

inline void Chunk::AddTopFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }, light));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(3 + offset);
    m_indices.emplace_back(2 + offset);
}

inline void Chunk::AddBottomFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }, light));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(3 + offset);
}

inline void Chunk::AddLeftFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 1.0f, 0.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 0.0f, 1.0f, idx }, light));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(3 + offset);
    m_indices.emplace_back(2 + offset);
}

inline void Chunk::AddRightFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices.size());

    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 0.0f, 0.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }, light));
    m_vertices.emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 1.0f, 1.0f, idx }, light));

    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(1 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(0 + offset);
    m_indices.emplace_back(2 + offset);
    m_indices.emplace_back(3 + offset);
}

inline void Chunk::ShrinkAirBlocks() noexcept
{

    const size_t prevY = (static_cast<size_t>(m_blocks.size()) / SQ_WIDTH) - 1;
    bool isAirOnly = true;
    for (size_t i = prevY * SQ_WIDTH; i < m_blocks.size(); i++)
    {
        if (m_blocks[i].GetId() != BlockId::Air)
        {
            isAirOnly = false;
        }
    }
    if (isAirOnly)
    {
        const size_t newSize = m_blocks.size() - static_cast<size_t>(SQ_WIDTH);
        m_blocks.resize(newSize);
    }
}

inline size_t Chunk::GetIdxFromCoords(int x, int y, int z) const noexcept
{
    return static_cast<size_t>(x + z * WIDTH + y * SQ_WIDTH);
}

int Chunk::GetSunlight(int x, int y, int z) const noexcept
{
    return (m_lightMap[GetIdxFromCoords(x, y, z)] >> 12) & 0xF;
}

void Chunk::SetSunlight(int x, int y, int z, int val) noexcept
{
    const size_t idx = GetIdxFromCoords(x, y, z);
    m_lightMap[idx] = static_cast<uint16_t>((m_lightMap[idx] & 0x0FFF) | (val << 12));
}

int Chunk::GetRedLight(int x, int y, int z) const noexcept
{
    const size_t idx = GetIdxFromCoords(x, y, z);
    if (idx >= m_lightMap.size())
    {
        return 0;
    }
    return (m_lightMap[idx] >> 8) & 0xF;
}

int Chunk::GetRedLight(size_t idx) const noexcept
{
    if (idx >= m_lightMap.size())
    {
        return 0;
    }
    return (m_lightMap[idx] >> 8) & 0xF;
}

void Chunk::SetRedLight(int x, int y, int z, int val) noexcept
{
    const size_t idx = GetIdxFromCoords(x, y, z);
    if (idx >= m_lightMap.size())
    {
        const int prevY = (static_cast<int>(m_lightMap.size()) / SQ_WIDTH) - 1;
        if (prevY < y)
        {
            const size_t newSize = m_lightMap.size() + static_cast<size_t>((y - prevY) * SQ_WIDTH);
            m_lightMap.resize(newSize);
        }
    }
    m_lightMap[idx] = static_cast<uint16_t>((m_lightMap[idx] & 0xF0FF) | (val << 8));
}

int Chunk::GetGreenLight(int x, int y, int z) const noexcept
{
    const size_t idx = GetIdxFromCoords(x, y, z);
    if (idx >= m_lightMap.size())
    {
        return 0;
    }
    return (m_lightMap[GetIdxFromCoords(x, y, z)] >> 4) & 0xF;
}

void Chunk::SetGreenLight(int x, int y, int z, int val) noexcept
{
    const size_t idx = GetIdxFromCoords(x, y, z);
    if (idx >= m_lightMap.size())
    {
        const int prevY = (static_cast<int>(m_lightMap.size()) / SQ_WIDTH) - 1;
        if (prevY < y)
        {
            const size_t newSize = m_lightMap.size() + static_cast<size_t>((y - prevY) * SQ_WIDTH);
            m_lightMap.resize(newSize);
        }
    }
    m_lightMap[idx] = static_cast<uint16_t>((m_lightMap[idx] & 0xFF0F) | (val << 4));
}

int Chunk::GetBlueLight(int x, int y, int z) const noexcept
{
    const size_t idx = GetIdxFromCoords(x, y, z);
    if (idx >= m_lightMap.size())
    {
        return 0;
    }
    return m_lightMap[GetIdxFromCoords(x, y, z)] & 0xF;
}

void Chunk::SetBlueLight(int x, int y, int z, int val) noexcept
{
    const size_t idx = GetIdxFromCoords(x, y, z);
    if (idx >= m_lightMap.size())
    {
        const int prevY = (static_cast<int>(m_lightMap.size()) / SQ_WIDTH) - 1;
        if (prevY < y)
        {
            const size_t newSize = m_lightMap.size() + static_cast<size_t>((y - prevY) * SQ_WIDTH);
            m_lightMap.resize(newSize);
        }
    }
    m_lightMap[idx] = static_cast<uint16_t>((m_lightMap[idx] & 0xFFF0) | val);
}

uint16_t Chunk::GetLightAt(int x, int y, int z) noexcept
{
    const size_t idx = GetIdxFromCoords(x, y, z);
    if (idx >= m_lightMap.size())
    {
        return 0;
    }
    return m_lightMap[idx];
}

void Chunk::UpdateMeshWithoutBuffers(BlockManager& blockManager, 
    std::shared_ptr<Chunk> leftChunk, std::shared_ptr<Chunk> rightChunk,
    std::shared_ptr<Chunk> frontChunk, std::shared_ptr<Chunk> backChunk)
{
    m_vertices.clear();
    m_indices.clear();
    const int maxY = static_cast<int>(m_blocks.size()) / SQ_WIDTH;
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

                if (z == 0)
                {
                    if (!(frontChunk && frontChunk->HasBlockAt(x, y, WIDTH - 1)))
                    {
                        AddFrontFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Front), frontChunk ? frontChunk->GetLightAt(x, y, WIDTH - 1) : 0);
                    }
                }
                else
                {
                    if (!HasBlockAt(x, y, z - 1))
                    {
                        AddFrontFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Front), GetLightAt(x, y, z - 1));
                    }
                }
                if (z == WIDTH - 1)
                {
                    if (!(backChunk && backChunk->HasBlockAt(x, y, 0)))
                    {
                        AddBackFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Back), backChunk ? backChunk->GetLightAt(x, y, 0) : 0);
                    }
                }
                else
                {
                    if (!HasBlockAt(x, y, z + 1))
                    {
                        AddBackFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Back), GetLightAt(x, y, z + 1));
                    }
                }
                if (pos.y == HEIGHT - 1 || !HasBlockAt(x, y + 1, z))
                {
                    AddTopFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Top), GetLightAt(x, y + 1, z));
                }
                if (pos.y == 0 || !HasBlockAt(x, y - 1, z))
                {
                    AddBottomFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Bottom), GetLightAt(x, y - 1, z));
                }
                if (x == 0)
                {
                    if (!(leftChunk && leftChunk->HasBlockAt(WIDTH - 1, y, z)))
                    {
                        AddLeftFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Left), leftChunk ? leftChunk->GetLightAt(WIDTH - 1, y, 0) : 0);
                    }
                }
                else
                {
                    if (!HasBlockAt(x - 1, y, z))
                    {
                        AddLeftFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Left), GetLightAt(x - 1, y, z));
                    }
                }
                if (x == WIDTH - 1)
                {
                    if (!(rightChunk && rightChunk->HasBlockAt(0, y, z)))
                    {
                        AddRightFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Right), rightChunk ? rightChunk->GetLightAt(0, y, z) : 0);
                    }
                }
                else
                {
                    if (!HasBlockAt(x + 1, y, z))
                    {
                        AddRightFace(blockPos, block.GetFaceTexture(Block::BlockFaces::Right), GetLightAt(x + 1, y, z));
                    }
                    
                }
            }
        }
    }
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
