#pragma once

#include "Vertex.h"
#include "ChunkBlock.h"
#include "BlockManager.h"
#include "WorldPos.hpp"

class Chunk
{
public:
	Chunk(WorldPos& worldPos);

	static const int WIDTH = 32;
	static const int HEIGHT = 32;
	static const int DEPTH = 32;
	static constexpr int VOLUME = WIDTH * HEIGHT * HEIGHT;

	const std::unordered_map<WorldPos, ChunkBlock, WorldPosHash>& GetBlocks() const noexcept;

	void FillWith();

	void UpdateMesh(ID3D11Device* device, BlockManager& blockManager);
	void UpdateMeshWithoutBuffers(BlockManager blockManager);

	void UpdateBuffers(ID3D11Device* device);

	bool HasBlockAt(WorldPos& pos);

	WorldPos& GetPos() noexcept;

	std::vector<Vertex>& GetVertices();
	std::vector<UINT>& GetIndices();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

private:
	void AddFrontFace(DirectX::XMFLOAT3 pos, std::string texture);
	void AddBackFace(DirectX::XMFLOAT3 pos, std::string texture);
	void AddTopFace(DirectX::XMFLOAT3 pos, std::string texture);
	void AddBottomFace(DirectX::XMFLOAT3 pos, std::string texture);
	void AddLeftFace(DirectX::XMFLOAT3 pos, std::string texture);
	void AddRightFace(DirectX::XMFLOAT3 pos, std::string texture);

private:
	std::unordered_map<WorldPos, ChunkBlock, WorldPosHash> m_blocks;

	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;

	WorldPos m_worldPos;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	void BuildVertexBuffer(ID3D11Device* device);
	void BuildIndexBuffer(ID3D11Device* device);
};

