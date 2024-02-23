#pragma once
#include <cstdint>
#include <vector>

class Chunk;

class Lightmap
{
public:
	Lightmap();

	inline std::uint16_t GetLight(int x, int y, int z) noexcept
	{
		const size_t idx = GetIdx(x, y, z);
		return GetLight(idx);
	}
	inline std::uint16_t GetLight(size_t idx) const noexcept
	{
		if (idx >= m_lightMap.size())
		{
			return 0;
		}
		return m_lightMap[idx];
	};

	inline int GetChannel(int x, int y, int z, int channel) noexcept
	{
		const size_t idx = GetIdx(x, y, z);
		return GetChannel(idx, channel);
	};
	inline int GetChannel(size_t idx, int channel) noexcept
	{
		if (idx >= m_lightMap.size())
		{
			return 0;
		}
		return (m_lightMap[idx] >> (channel * 4)) & 0xF;
	};
	int GetS(int x, int y, int z) noexcept;
	int GetR(int x, int y, int z) noexcept;
	int GetG(int x, int y, int z) noexcept;
	int GetB(int x, int y, int z) noexcept;

	inline void SetChannel(int x, int y, int z, int channel, int val) noexcept
	{
		const size_t idx = GetIdx(x, y, z);
		if (idx >= m_lightMap.size())
		{
			ExtendMapTo(y);
		}
		m_lightMap[idx] = static_cast<std::uint16_t>((m_lightMap[idx] & ~(0xF << (channel * 4))) | (val << (channel * 4)));
	};
	void SetS(int x, int y, int z, int val) noexcept;
	void SetR(int x, int y, int z, int val) noexcept;
	void SetG(int x, int y, int z, int val) noexcept;
	void SetB(int x, int y, int z, int val) noexcept;

private:
	size_t GetIdx(int x, int y, int z) noexcept;
	void ExtendMapTo(int y) noexcept;

private:
	std::vector<std::uint16_t> m_lightMap;
};

