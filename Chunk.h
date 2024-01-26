#pragma once

#include "Vertex.h"
#include "ChunkBlock.h"
#include "BlockManager.h"
#include "WorldPos.hpp"
#include "BlockId.h"

class Chunk
{
public:
	Chunk(ChunkPos& chunkPos);

	static const int WIDTH = 16;
	static const int SQ_WIDTH = WIDTH * WIDTH;
	static const int HEIGHT = 256;

	void RemoveBlock(int x, int y, int z) noexcept;

	void AddBlock(int x, int y, int z, BlockId blockId) noexcept;
	void AddBlock(int x, int y, int z, ChunkBlock block) noexcept;

	ChunkBlock GetBlock(int x, int y, int z) noexcept;

	void UpdateMeshWithoutBuffers(BlockManager& blockManager, 
		std::shared_ptr<Chunk> leftChunk, std::shared_ptr<Chunk> rightChunk, 
		std::shared_ptr<Chunk> frontChunk, std::shared_ptr<Chunk> backChunk);

	void UpdateBuffers(ID3D11Device* device);

	inline bool HasBlockAt(int x, int y, int z) const noexcept;

	bool IsModified() const noexcept;
	void SetIsModified(bool isModified) noexcept;

	bool ShouldRender() const noexcept;
	void SetShouldRender(bool shouldRender) noexcept;

	int GetSunlight(int x, int y, int z) const noexcept;
	void SetSunlight(int x, int y, int z, int val) noexcept;
	int GetRedLight(int x, int y, int z) const noexcept;
	int GetRedLight(size_t idx) const noexcept;
	void SetRedLight(int x, int y, int z, int val) noexcept;
	int GetGreenLight(int x, int y, int z) const noexcept;
	void SetGreenLight(int x, int y, int z, int val) noexcept;
	int GetBlueLight(int x, int y, int z) const noexcept;
	void SetBlueLight(int x, int y, int z, int val) noexcept;
	uint16_t GetLightAt(int x, int y, int z) noexcept;

	ChunkPos& GetPos() noexcept;

	std::vector<Vertex>& GetVertices();
	std::vector<UINT>& GetIndices();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

private:
	inline void AddFrontFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddBackFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddTopFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddBottomFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddLeftFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;
	inline void AddRightFace(DirectX::XMFLOAT3& pos, std::string& texture, uint16_t light) noexcept;

	inline size_t GetIdxFromCoords(int x, int y, int z) const noexcept;

private:
	bool m_isModified = true;
	bool m_shouldRender = false;
	std::vector<ChunkBlock> m_blocks;
	std::vector<uint16_t> m_lightMap;

	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;

	ChunkPos m_worldPos;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	void BuildVertexBuffer(ID3D11Device* device);
	void BuildIndexBuffer(ID3D11Device* device);
};

