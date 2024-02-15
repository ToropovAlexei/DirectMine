#pragma once

#include "Vertex.h"
#include "ChunkBlock.h"
#include "BlockManager.h"
#include "WorldPos.hpp"
#include "BlockId.h"
#include "Lightmap.h"

class Chunk
{
public:
	Chunk(int x, int z);

	static const int WIDTH = 16;
	static const int SQ_WIDTH = WIDTH * WIDTH;
	static const int HEIGHT = 256;
	static const int LAST_BLOCK_IDX = WIDTH - 1;
	static const int HIGHEST_BLOCK_IDX = HEIGHT - 1;

	inline void SetBlock(int x, int y, int z, BlockId blockId) noexcept
	{
		SetBlock(x, y, z, ChunkBlock(blockId));
	}
	void SetBlock(int x, int y, int z, ChunkBlock block) noexcept;

	ChunkBlock GetBlock(int x, int y, int z) noexcept;

	void UpdateMeshWithoutBuffers(BlockManager& blockManager, 
		std::shared_ptr<Chunk> leftChunk, std::shared_ptr<Chunk> rightChunk, 
		std::shared_ptr<Chunk> frontChunk, std::shared_ptr<Chunk> backChunk) noexcept;

	void UpdateBuffers(ID3D11Device* device);

	void UpdateSunlight(BlockManager& blockManager) noexcept;

	inline bool HasBlockAt(int x, int y, int z) const noexcept
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
	};
	inline bool HasBlockAt(size_t idx) const noexcept
	{
		if (idx >= m_blocks.size())
		{
			return false;
		}
		return m_blocks[idx].GetId() != BlockId::Air;
	};
	inline bool HasOpaqueBlockAt(int x, int y, int z, BlockManager& blockManager) const noexcept;
	inline bool HasOpaqueBlockAt(size_t idx, BlockManager& blockManager) const noexcept;

	inline bool IsModified() const noexcept { return m_isModified; };
	inline void SetIsModified(bool isModified) noexcept { m_isModified = isModified; };

	inline bool ShouldRender() const noexcept { return m_shouldRender; };
	inline void SetShouldRender(bool shouldRender) noexcept { m_shouldRender = shouldRender; };

	bool CanAddFace(int x, int y, int z, BlockManager& blockManager, ChunkBlock& block) noexcept;
	bool CanAddFace(size_t idx, BlockManager& blockManager, ChunkBlock& block) noexcept;

	inline Lightmap& GetLightmapRef() noexcept { return m_lightMap; };

	inline uint16_t GetLightAt(int x, int y, int z) const noexcept
	{
		size_t idx = GetIdxFromCoords(x, y, z);
		
		return GetLightAt(idx);
	};

	inline uint16_t GetLightAt(size_t idx) const noexcept
	{
		// Если индекс выходит за рамки всех блоков, значит там есть солнечный свет,
		// указываем его как 0xF000, а остальные каналы берем из карты
		if (idx > m_blocks.size())
		{
			return m_lightMap.GetLight(idx) | 0xF000;
		}
		return m_lightMap.GetLight(idx);
	};

	inline int GetMaxY() const noexcept
	{
		return (static_cast<int>(m_blocks.size()) / SQ_WIDTH) - 1;
	};

	inline int GetX() const noexcept { return m_x; };
	inline int GetZ() const noexcept { return m_z; };

	inline size_t GetIndicesCount(int drawGroup) const noexcept { return m_indicesCount[drawGroup]; };
	inline Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer(int drawGroup) noexcept { return m_vertexBuffer[drawGroup]; };
	inline Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer(int drawGroup) noexcept { return m_indexBuffer[drawGroup]; };

private:
	inline void AddFrontFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddBackFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddTopFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddBottomFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddLeftFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddRightFace(int drawGroup, DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;

	inline void ShrinkAirBlocks() noexcept;

	inline size_t GetIdxFromCoords(int x, int y, int z) const noexcept;

private:
	int m_x;
	int m_z;
	bool m_isModified = true;
	bool m_shouldRender = false;
	std::vector<ChunkBlock> m_blocks;
	Lightmap m_lightMap;

	std::array<std::vector<Vertex>, 4> m_vertices;
	std::array<std::vector<UINT>, 4> m_indices;
	std::array<size_t, 4> m_indicesCount;

	std::array<Microsoft::WRL::ComPtr<ID3D11Buffer>, 4> m_vertexBuffer;
	std::array<Microsoft::WRL::ComPtr<ID3D11Buffer>, 4> m_indexBuffer;

	void BuildVertexBuffer(ID3D11Device* device);
	void BuildIndexBuffer(ID3D11Device* device);
};

