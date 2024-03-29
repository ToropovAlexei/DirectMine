#include "pch.h"
#include "Lightmap.h"
#include "Chunk.h"

Lightmap::Lightmap()
{
	m_lightMap.resize(Chunk::SQ_WIDTH);
}

int Lightmap::GetS(int x, int y, int z) noexcept
{
    const size_t idx = GetIdx(x, y, z);
    if (idx >= m_lightMap.size())
    {
        return 0;
    }
    return (m_lightMap[idx] >> 12) & 0xF;
}

int Lightmap::GetR(int x, int y, int z) noexcept
{
    const size_t idx = GetIdx(x, y, z);
    if (idx >= m_lightMap.size())
    {
        return 0;
    }
    return (m_lightMap[idx] >> 8) & 0xF;
}

int Lightmap::GetG(int x, int y, int z) noexcept
{
    const size_t idx = GetIdx(x, y, z);
    if (idx >= m_lightMap.size())
    {
        return 0;
    }
    return (m_lightMap[idx] >> 4) & 0xF;
}

int Lightmap::GetB(int x, int y, int z) noexcept
{
    const size_t idx = GetIdx(x, y, z);
    if (idx >= m_lightMap.size())
    {
        return 0;
    }
    return m_lightMap[idx] & 0xF;
}

void Lightmap::SetS(int x, int y, int z, int val) noexcept
{
    const size_t idx = GetIdx(x, y, z);
    if (idx >= m_lightMap.size())
    {
        ExtendMapTo(y);
    }
    m_lightMap[idx] = static_cast<uint16_t>((m_lightMap[idx] & 0x0FFF) | (val << 12));
}

void Lightmap::SetR(int x, int y, int z, int val) noexcept
{
    const size_t idx = GetIdx(x, y, z);
    if (idx >= m_lightMap.size())
    {
        ExtendMapTo(y);
    }
    m_lightMap[idx] = static_cast<uint16_t>((m_lightMap[idx] & 0xF0FF) | (val << 8));
}

void Lightmap::SetG(int x, int y, int z, int val) noexcept
{
    const size_t idx = GetIdx(x, y, z);
    if (idx >= m_lightMap.size())
    {
        ExtendMapTo(y);
    }
    m_lightMap[idx] = static_cast<uint16_t>((m_lightMap[idx] & 0xFF0F) | (val << 4));
}

void Lightmap::SetB(int x, int y, int z, int val) noexcept
{
    const size_t idx = GetIdx(x, y, z);
    if (idx >= m_lightMap.size())
    {
        ExtendMapTo(y);
    }
    m_lightMap[idx] = static_cast<uint16_t>((m_lightMap[idx] & 0xFFF0) | val);
}

void Lightmap::ExtendMapTo(int y) noexcept
{
    const int prevY = (static_cast<int>(m_lightMap.size()) / Chunk::SQ_WIDTH) - 1;
    if (prevY < y)
    {
        const size_t newSize = m_lightMap.size() + static_cast<size_t>((y - prevY) * Chunk::SQ_WIDTH);
        m_lightMap.resize(newSize);
    }
}

size_t Lightmap::GetIdx(int x, int y, int z) noexcept
{
    return static_cast<size_t>(x + z * Chunk::WIDTH + y * Chunk::SQ_WIDTH);
};