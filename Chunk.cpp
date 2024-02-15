#include "pch.h"
#include "Chunk.h"
#include "UVCoords.h"
#include "TextureAtlas.h"
#include "MathUtils.h"
#include "Lightmap.h"

Chunk::Chunk(int x, int z) :
    m_x(x), m_z(z)
{
    m_blocks.resize(static_cast<size_t>(SQ_WIDTH));
    // —реднее значение генератора мира = 64
    m_blocks.reserve(SQ_WIDTH * 64);
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
            // Ѕерем максимальный размер, т.к. в lightMap может быть больше элементов
            //m_lightMap.resize(std::max(newSize, m_lightMap.size()));
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

inline void Chunk::AddFrontFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices[drawGroup].size());

    m_vertices[drawGroup].emplace_back(Vertex({0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z}, {0.0f, 1.0f, idx}, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 1.0f, 0.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 0.0f, 0.0f, idx }, light));

    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
    m_indices[drawGroup].emplace_back(1 + offset);
    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(3 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
}

inline void Chunk::AddBackFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices[drawGroup].size());

    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 1.0f, 1.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 0.0f, 1.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }, light));

    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(1 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
    m_indices[drawGroup].emplace_back(3 + offset);
}

inline void Chunk::AddTopFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices[drawGroup].size());

    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }, light));

    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
    m_indices[drawGroup].emplace_back(1 + offset);
    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(3 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
}

inline void Chunk::AddBottomFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices[drawGroup].size());

    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }, light));

    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(1 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
    m_indices[drawGroup].emplace_back(3 + offset);
}

inline void Chunk::AddLeftFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices[drawGroup].size());

    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 1.0f, 1.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 1.0f, 0.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 0.0f, 0.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 0.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 0.0f, 1.0f, idx }, light));

    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
    m_indices[drawGroup].emplace_back(1 + offset);
    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(3 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
}

inline void Chunk::AddRightFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept
{
    float idx = TextureAtlas::GetTextureIdx(texture);
    UINT offset = static_cast<UINT>(m_vertices[drawGroup].size());

    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 0.0f + pos.z }, { 0.0f, 1.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 0.0f + pos.z }, { 0.0f, 0.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 1.0f + pos.y, 1.0f + pos.z }, { 1.0f, 0.0f, idx }, light));
    m_vertices[drawGroup].emplace_back(Vertex({ 1.0f + pos.x, 0.0f + pos.y, 1.0f + pos.z }, { 1.0f, 1.0f, idx }, light));

    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(1 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
    m_indices[drawGroup].emplace_back(0 + offset);
    m_indices[drawGroup].emplace_back(2 + offset);
    m_indices[drawGroup].emplace_back(3 + offset);
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

void Chunk::UpdateMeshWithoutBuffers(BlockManager& blockManager,
    std::shared_ptr<Chunk> leftChunk, std::shared_ptr<Chunk> rightChunk,
    std::shared_ptr<Chunk> frontChunk, std::shared_ptr<Chunk> backChunk) noexcept
{

    // Ёти числа вз€ты из среднего значени€ в чанках
    for (int drawGroup = 0; drawGroup < 4; drawGroup++)
    {
        assert(m_vertices[drawGroup].empty());
        assert(m_indices[drawGroup].empty());
        m_vertices[drawGroup].reserve(20000);
        m_indices[drawGroup].reserve(20000);
    }
    const int maxY = static_cast<int>(m_blocks.size()) / SQ_WIDTH;
    const int chunkX = m_x * Chunk::WIDTH;
    const int chunkZ = m_z * Chunk::WIDTH;
    size_t prevIdx = 0;
    for (int y = 0; y < maxY; y++)
    {
        for (int z = 0; z < WIDTH; z++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                const size_t idx = prevIdx++;
                BlockId blockId = m_blocks[idx].GetId();
                if (blockId == BlockId::Air)
                {
                    continue;
                }
                Block& block = blockManager.GetBlockById(blockId);
                DirectX::XMFLOAT3 blockPos = DirectX::XMFLOAT3(static_cast<float>(x + chunkX), static_cast<float>(y), static_cast<float>(z + chunkZ));

                if (z == 0)
                {
                    if (!frontChunk || frontChunk->CanAddFace(x, y, LAST_BLOCK_IDX, blockManager, m_blocks[idx]))
                    {
                        AddFrontFace(block.GetDrawGroup(), blockPos, block.GetFaceTexture(Block::BlockFaces::Front), frontChunk ? frontChunk->GetLightAt(x, y, LAST_BLOCK_IDX) : 0xF000);
                    }
                }
                else
                {
                    if (CanAddFace(idx - WIDTH, blockManager, m_blocks[idx]))
                    {
                        AddFrontFace(block.GetDrawGroup(), blockPos, block.GetFaceTexture(Block::BlockFaces::Front), GetLightAt(idx - WIDTH));
                    }
                }
                if (z == LAST_BLOCK_IDX)
                {
                    if (!backChunk || backChunk->CanAddFace(x, y, 0, blockManager, m_blocks[idx]))
                    {
                        AddBackFace(block.GetDrawGroup(), blockPos, block.GetFaceTexture(Block::BlockFaces::Back), backChunk ? backChunk->GetLightAt(x, y, 0) : 0xF000);
                    }
                }
                else
                {
                    if (CanAddFace(idx + WIDTH, blockManager, m_blocks[idx]))
                    {
                        AddBackFace(block.GetDrawGroup(), blockPos, block.GetFaceTexture(Block::BlockFaces::Back), GetLightAt(idx + WIDTH));
                    }
                }
                if (y == HIGHEST_BLOCK_IDX || CanAddFace(idx + SQ_WIDTH, blockManager, m_blocks[idx]))
                {
                    AddTopFace(block.GetDrawGroup(), blockPos, block.GetFaceTexture(Block::BlockFaces::Top), GetLightAt(idx + SQ_WIDTH));
                }
                if (y == 0 || CanAddFace(idx - SQ_WIDTH, blockManager, m_blocks[idx]))
                {
                    AddBottomFace(block.GetDrawGroup(), blockPos, block.GetFaceTexture(Block::BlockFaces::Bottom), GetLightAt(idx - SQ_WIDTH));
                }
                if (x == 0)
                {
                    if (!leftChunk || leftChunk->CanAddFace(LAST_BLOCK_IDX, y, z, blockManager, m_blocks[idx]))
                    {
                        AddLeftFace(block.GetDrawGroup(), blockPos, block.GetFaceTexture(Block::BlockFaces::Left), leftChunk ? leftChunk->GetLightAt(LAST_BLOCK_IDX, y, z) : 0xF000);
                    }
                }
                else
                {
                    if (CanAddFace(idx - 1, blockManager, m_blocks[idx]))
                    {
                        AddLeftFace(block.GetDrawGroup(), blockPos, block.GetFaceTexture(Block::BlockFaces::Left), GetLightAt(idx - 1));
                    }
                }
                if (x == LAST_BLOCK_IDX)
                {
                    if (!rightChunk || rightChunk->CanAddFace(0, y, z, blockManager, m_blocks[idx]))
                    {
                        AddRightFace(block.GetDrawGroup(), blockPos, block.GetFaceTexture(Block::BlockFaces::Right), rightChunk ? rightChunk->GetLightAt(0, y, z) : 0xF000);
                    }
                }
                else
                {
                    if (CanAddFace(idx + 1, blockManager, m_blocks[idx]))
                    {
                        AddRightFace(block.GetDrawGroup(), blockPos, block.GetFaceTexture(Block::BlockFaces::Right), GetLightAt(idx + 1));
                    }

                }
            }
        }
    }
}

inline bool Chunk::HasOpaqueBlockAt(int x, int y, int z, BlockManager& blockManager) const noexcept
{
    assert(x < Chunk::WIDTH);
    //assert(chunkPos.y < Chunk::HEIGHT);
    assert(z < Chunk::WIDTH);
    const size_t idx = GetIdxFromCoords(x, y, z);
    if (idx >= m_blocks.size())
    {
        return false;
    }
    return HasOpaqueBlockAt(idx, blockManager);
}

inline bool Chunk::HasOpaqueBlockAt(size_t idx, BlockManager& blockManager) const noexcept
{
    if (idx >= m_blocks.size())
    {
        return false;
    }
    return blockManager.GetBlockById(m_blocks[idx].GetId()).IsOpaque();
}

bool Chunk::CanAddFace(int x, int y, int z, BlockManager& blockManager, ChunkBlock& block) noexcept
{
    assert(x < Chunk::WIDTH);
    //assert(chunkPos.y < Chunk::HEIGHT);
    assert(z < Chunk::WIDTH);
    const size_t idx = GetIdxFromCoords(x, y, z);
    if (idx >= m_blocks.size())
    {
        return true;
    }
    return CanAddFace(idx, blockManager, block);
}

bool Chunk::CanAddFace(size_t idx, BlockManager& blockManager, ChunkBlock& block) noexcept
{
    if (idx >= m_blocks.size())
    {
        return true;
    }
    ChunkBlock& adjBlock = m_blocks[idx];
    if (adjBlock.GetId() == BlockId::Air)
    {
        return true;
    }
    Block& adjBlockData = blockManager.GetBlockById(adjBlock.GetId());

    return !adjBlockData.IsOpaque() && block.GetId() != adjBlock.GetId();
}

void Chunk::UpdateBuffers(ID3D11Device* device)
{
    BuildVertexBuffer(device);
    BuildIndexBuffer(device);
    for (int drawGroup = 0; drawGroup < 4; drawGroup++)
    {
        m_indicesCount[drawGroup] = m_indices[drawGroup].size();
        m_vertices[drawGroup].clear();
        m_indices[drawGroup].clear();
        m_vertices[drawGroup].shrink_to_fit();
        m_indices[drawGroup].shrink_to_fit();
    }
}

void Chunk::UpdateSunlight(BlockManager& blockManager) noexcept
{
    const int maxY = GetMaxY();
    for (int x = 0; x < WIDTH; x++)
    {
        for (int z = 0; z < WIDTH; z++)
        {
            int y = maxY + 1;
            while (y > 0)
            {
                if (blockManager.GetBlockById(GetBlock(x, y, z).GetId()).IsOpaque())
                {
                    break;
                }
                m_lightMap.SetS(x, y, z, 15);
                y--;
            }
        }
    }
}

void Chunk::BuildVertexBuffer(ID3D11Device* device)
{
    for (int drawGroup = 0; drawGroup < 4; drawGroup++)
    {
        if (m_vertices[drawGroup].empty())
        {
            continue;
        }
        D3D11_BUFFER_DESC bd = {};
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.CPUAccessFlags = 0u;
        bd.MiscFlags = 0u;
        bd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * m_vertices[drawGroup].size());
        bd.StructureByteStride = sizeof(Vertex);
        D3D11_SUBRESOURCE_DATA sd = {};
        sd.pSysMem = m_vertices[drawGroup].data();
        device->CreateBuffer(&bd, &sd, &m_vertexBuffer[drawGroup]);
    }
}

void Chunk::BuildIndexBuffer(ID3D11Device* device)
{
    for (int drawGroup = 0; drawGroup < 4; drawGroup++)
    {
        if (m_indices[drawGroup].empty())
        {
            continue;
        }
        D3D11_BUFFER_DESC ibd = {};
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_DEFAULT;
        ibd.CPUAccessFlags = 0u;
        ibd.MiscFlags = 0u;
        ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_indices[drawGroup].size());
        ibd.StructureByteStride = sizeof(UINT);
        D3D11_SUBRESOURCE_DATA isd = {};
        isd.pSysMem = m_indices[drawGroup].data();
        device->CreateBuffer(&ibd, &isd, &m_indexBuffer[drawGroup]);
    }
}
