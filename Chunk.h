#pragma once

#include "Cube.h"
#include "Vertex.h"

class Chunk
{
public:
	struct BlockPos
	{
		float x;
		float y;
		float z;

		bool operator==(const BlockPos& other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}
	};

	struct BlockPosHash
	{
		std::size_t operator()(const BlockPos& pos) const
		{
			// Преобразование значений типа float в биты
			std::size_t xHash = std::hash<float>{}(pos.x);
			std::size_t yHash = std::hash<float>{}(pos.y);
			std::size_t zHash = std::hash<float>{}(pos.z);

			// Комбинирование хэшей с помощью побитового исключающего ИЛИ (XOR)
			std::size_t combinedHash = xHash ^ yHash ^ zHash;

			return combinedHash;
		}
	};
public:
	Chunk();

	static const int WIDTH = 32;
	static const int HEIGHT = 32;
	static const int DEPTH = 32;
	static constexpr int VOLUME = WIDTH * HEIGHT * HEIGHT;

	const std::unordered_map<BlockPos, Cube, BlockPosHash>& GetBlocks() const noexcept;

	void FillWith();

	void UpdateMesh(ID3D11Device* device);

	std::vector<Vertex>& GetVertices();
	std::vector<UINT>& GetIndices();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

private:
	void addFrontFace(DirectX::XMFLOAT3 pos);
	void addBackFace(DirectX::XMFLOAT3 pos);
	void addTopFace(DirectX::XMFLOAT3 pos);
	void addBottomFace(DirectX::XMFLOAT3 pos);
	void addLeftFace(DirectX::XMFLOAT3 pos);
	void addRightFace(DirectX::XMFLOAT3 pos);
	std::unordered_map<BlockPos, Cube, BlockPosHash> m_blocks;

	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	void BuildVertexBuffer(ID3D11Device* device);
	void BuildIndexBuffer(ID3D11Device* device);
};

