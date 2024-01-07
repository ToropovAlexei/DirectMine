#pragma once

#include "Cube.h"

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
			// �������������� �������� ���� float � ����
			std::size_t xHash = std::hash<float>{}(pos.x);
			std::size_t yHash = std::hash<float>{}(pos.y);
			std::size_t zHash = std::hash<float>{}(pos.z);

			// �������������� ����� � ������� ���������� ������������ ��� (XOR)
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

	DirectX::GeometricPrimitive::VertexCollection& GetVertices();
	DirectX::GeometricPrimitive::IndexCollection& GetIndices();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

private:
	std::unordered_map<BlockPos, Cube, BlockPosHash> m_blocks;

	DirectX::GeometricPrimitive::VertexCollection m_vertices;
	DirectX::GeometricPrimitive::IndexCollection m_indices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	void BuildVertexBuffer(ID3D11Device* device);
	void BuildIndexBuffer(ID3D11Device* device);
};
