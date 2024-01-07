#include "pch.h"
#include "Chunk.h"

Chunk::Chunk()
{

}

const std::unordered_map<Chunk::BlockPos, Cube, Chunk::BlockPosHash>& Chunk::GetBlocks() const noexcept
{
	return m_blocks;
}

void Chunk::FillWith()
{
	for (size_t x = 0; x < Chunk::WIDTH; x++)
	{
		for (size_t y = 0; y < Chunk::HEIGHT; y++)
		{
			for (size_t z = 0; z < Chunk::DEPTH; z++)
			{
				BlockPos pos = BlockPos(x, y, z);
				m_blocks.insert({ pos, Cube() });
			}
		}
	}
    UpdateMesh();
}

DirectX::GeometricPrimitive::VertexCollection& Chunk::GetVertices()
{
    return m_vertices;
}

DirectX::GeometricPrimitive::IndexCollection& Chunk::GetIndices()
{
    return m_indices;
}

void Chunk::UpdateMesh()
{
	for (auto& blockPair : m_blocks)
	{
        DirectX::GeometricPrimitive::VertexCollection vertices;
        DirectX::GeometricPrimitive::IndexCollection indices;

        DirectX::GeometricPrimitive::CreateCube(vertices, indices, 1.0f, false);

        // Back
        vertices[0].textureCoordinate = { 0.75f, 1.0f / 3.0f };
        vertices[1].textureCoordinate = { 0.75f, 2.0f / 3.0f };
        vertices[2].textureCoordinate = { 1.0f, 2.0f / 3.0f };
        vertices[3].textureCoordinate = { 1.0f, 1.0f / 3.0f };
        // Front
        vertices[4].textureCoordinate = { 0.25f, 1.0f / 3.0f };
        vertices[5].textureCoordinate = { 0.25f, 2.0f / 3.0f };
        vertices[6].textureCoordinate = { 0.5f, 2.0f / 3.0f };
        vertices[7].textureCoordinate = { 0.5f, 1.0f / 3.0f };
        // Right
        vertices[8].textureCoordinate = { 0.5f, 1.0f / 3.0f };
        vertices[9].textureCoordinate = { 0.5f, 2.0f / 3.0f };
        vertices[10].textureCoordinate = { 0.75f, 2.0f / 3.0f };
        vertices[11].textureCoordinate = { 0.75f, 1.0f / 3.0f };
        // Left
        vertices[12].textureCoordinate = { 0.0f, 1.0f / 3.0f };
        vertices[13].textureCoordinate = { 0.0f, 2.0f / 3.0f };
        vertices[14].textureCoordinate = { 0.25f, 2.0f / 3.0f };
        vertices[15].textureCoordinate = { 0.25f, 1.0f / 3.0f };
        // Top
        vertices[16].textureCoordinate = { 0.25f, 0.0f };
        vertices[17].textureCoordinate = { 0.25f, 1.0f / 3.0f };
        vertices[18].textureCoordinate = { 0.5f, 1.0f / 3.0f };
        vertices[19].textureCoordinate = { 0.5f, 0.0f };
        // Bottom
        vertices[20].textureCoordinate = { 0.25f, 2.0f / 3.0f };
        vertices[21].textureCoordinate = { 0.25f, 1.0f };
        vertices[22].textureCoordinate = { 0.5f, 1.0f };
        vertices[23].textureCoordinate = { 0.5f, 2.0f / 3.0f };
        m_vertices = vertices;
        m_indices = indices;
	}
}
