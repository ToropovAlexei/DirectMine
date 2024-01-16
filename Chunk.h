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
	static const int HEIGHT = 256;
	static const int DEPTH = 16;
	static constexpr int VOLUME = WIDTH * HEIGHT * HEIGHT;

	const std::unordered_map<WorldPos, ChunkBlock, WorldPosHash>& GetBlocks() const noexcept;

	void RemoveBlock(WorldPos& worldPos) noexcept;

	void AddBlock(WorldPos& worldPos, BlockId blockId) noexcept;
	void AddBlock(WorldPos& worldPos, ChunkBlock block) noexcept;

	std::optional<ChunkBlock> GetBlock(WorldPos& worldPos) noexcept;

	void UpdateMeshWithoutBuffers(BlockManager& blockManager, std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks);

	inline bool HasBlockInWorld(WorldPos& worldPos, std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks);

	void UpdateBuffers(ID3D11Device* device);

	inline bool HasBlockAt(WorldPos& pos) const noexcept;

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

private:
	bool m_isModified = true;
	bool m_shouldRender = false;
	std::unordered_map<WorldPos, ChunkBlock, WorldPosHash> m_blocks;

	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;

	ChunkPos m_worldPos;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	void BuildVertexBuffer(ID3D11Device* device);
	void BuildIndexBuffer(ID3D11Device* device);
};

