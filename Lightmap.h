#pragma once

class Lightmap
{
public:
	Lightmap();

	uint16_t GetLight(int x, int y, int z) noexcept;

	int GetS(int x, int y, int z) noexcept;
	int GetR(int x, int y, int z) noexcept;
	int GetG(int x, int y, int z) noexcept;
	int GetB(int x, int y, int z) noexcept;

	void SetS(int x, int y, int z, int val) noexcept;
	void SetR(int x, int y, int z, int val) noexcept;
	void SetG(int x, int y, int z, int val) noexcept;
	void SetB(int x, int y, int z, int val) noexcept;

private:
	inline size_t GetIdx(int x, int y, int z) noexcept;
	inline void ExtendMapTo(int y) noexcept;

private:
	std::vector<uint16_t> m_lightMap;
};

