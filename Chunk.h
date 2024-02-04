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

	inline bool HasBlockAt(int x, int y, int z) const noexcept;
	inline bool HasBlockAt(size_t idx) const noexcept;

	inline bool IsModified() const noexcept { return m_isModified; };
	inline void SetIsModified(bool isModified) noexcept { m_isModified = isModified; };

	inline bool ShouldRender() const noexcept { return m_shouldRender; };
	inline void SetShouldRender(bool shouldRender) noexcept { m_shouldRender = shouldRender; };

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

	inline std::vector<Vertex>& GetVertices() noexcept { return m_vertices; };
	inline std::vector<UINT>& GetIndices() noexcept { return m_indices; };
	inline Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() noexcept { return m_vertexBuffer; };
	inline Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() noexcept { return m_indexBuffer; };

private:
	inline void AddFrontFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddBackFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddTopFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddBottomFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddLeftFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddRightFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;

	inline void ShrinkAirBlocks() noexcept;

	inline size_t GetIdxFromCoords(int x, int y, int z) const noexcept;

private:
	int m_x;
	int m_z;
	bool m_isModified = true;
	bool m_shouldRender = false;
	std::vector<ChunkBlock> m_blocks;
	Lightmap m_lightMap;

	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	void BuildVertexBuffer(ID3D11Device* device);
	void BuildIndexBuffer(ID3D11Device* device);
};

