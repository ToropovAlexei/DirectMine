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

	std::optional<ChunkBlock> GetBlock(int x, int y, int z) noexcept;

	void UpdateMeshWithoutBuffers(BlockManager& blockManager, std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks);

	inline bool HasBlockInWorld(WorldPos& worldPos, std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks);

	void UpdateBuffers(ID3D11Device* device);

	inline bool HasBlockAt(int x, int y, int z) const noexcept;

	bool IsModified() const noexcept;
	void SetIsModified(bool isModified) noexcept;

	bool ShouldRender() const noexcept;
	void SetShouldRender(bool shouldRender) noexcept;

	ChunkPos& GetPos() noexcept;

	std::vector<Vertex>& GetVertices();
	std::vector<UINT>& GetIndices();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

private:
	inline void AddFrontFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept;
	inline void AddBackFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept;
	inline void AddTopFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept;
	inline void AddBottomFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept;
	inline void AddLeftFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept;
	inline void AddRightFace(DirectX::XMFLOAT3& pos, std::string& texture) noexcept;

	inline size_t GetIdxFromCoords(int x, int y, int z) const noexcept;

private:
	bool m_isModified = true;
	bool m_shouldRender = false;
	std::vector<ChunkBlock> m_blocks;

	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;

	ChunkPos m_worldPos;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	void BuildVertexBuffer(ID3D11Device* device);
	void BuildIndexBuffer(ID3D11Device* device);
};

