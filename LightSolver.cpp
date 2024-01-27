#include "pch.h"
#include "LightSolver.h"
#include "Chunk.h"

LightSolver::LightSolver(std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& chunks, BlockManager& blockManager, int channel):
	m_blockManager(blockManager),
	m_chunks(chunks),
	m_channel(channel)
{
}

void LightSolver::Add(LightNode node)
{
	m_addQueue.push(node);
}

void LightSolver::Remove(LightRemovalNode node)
{
	m_removeQueue.push(node);
}

void LightSolver::Solve()
{
	while (!m_removeQueue.empty())
	{
		LightRemovalNode node = m_removeQueue.front();
		node.chunk->SetIsModified(true);
		m_removeQueue.pop();
		int lightLevel = node.val;
		// Check left voxel
		if (node.x > 0)
		{
			int leftVoxelLightLevel = node.chunk->GetLightmapRef().GetChannel(node.x - 1, node.y, node.z, m_channel);
			if (leftVoxelLightLevel != 0 && leftVoxelLightLevel < lightLevel)
			{
				node.chunk->GetLightmapRef().SetChannel(node.x - 1, node.y, node.z, m_channel, 0);
				m_removeQueue.push({ node.x - 1, node.y, node.z, leftVoxelLightLevel, node.chunk });
			}
			else if (leftVoxelLightLevel >= lightLevel)
			{
				m_addQueue.push({ node.x - 1, node.y, node.z, node.chunk });
			}
		}
		else
		{
			ChunkPos leftChunkPos = node.chunk->GetPos() + ChunkPos(-Chunk::WIDTH, 0);
			auto chunk = m_chunks.find(leftChunkPos);
			if (chunk != m_chunks.end())
			{
				int leftVoxelLightLevel = chunk->second->GetLightmapRef().GetChannel(Chunk::WIDTH - 1, node.y, node.z, m_channel);
				if (leftVoxelLightLevel != 0 && leftVoxelLightLevel < lightLevel)
				{
					m_removeQueue.push({ Chunk::LAST_BLOCK_IDX, node.y, node.z, leftVoxelLightLevel, chunk->second });
				}
				else if (leftVoxelLightLevel >= lightLevel)
				{
					m_addQueue.push({ Chunk::LAST_BLOCK_IDX, node.y, node.z, chunk->second });
				}
			}
		}
	}

	while (!m_addQueue.empty())
	{
		LightNode node = m_addQueue.front();
		node.chunk->SetIsModified(true);
		m_addQueue.pop();
		int lightLevel = node.chunk->GetLightmapRef().GetChannel(node.x, node.y, node.z, m_channel);
		if (lightLevel < 2)
		{
			continue;
		}
		int nextVoxelLightlevel = lightLevel - 1;
		// Check left voxel
		if (node.x > 0)
		{
			TryAddLightNode(node.x - 1, node.y, node.z, nextVoxelLightlevel, node.chunk);
		}
		else
		{
			ChunkPos leftChunkPos = node.chunk->GetPos() + ChunkPos(-Chunk::WIDTH, 0);
			auto chunk = m_chunks.find(leftChunkPos);
			if (chunk != m_chunks.end())
			{
				TryAddLightNode(Chunk::LAST_BLOCK_IDX, node.y, node.z, nextVoxelLightlevel, chunk->second);
			}
		}
		// Check right voxel
		if (node.x < Chunk::LAST_BLOCK_IDX)
		{
			TryAddLightNode(node.x + 1, node.y, node.z, nextVoxelLightlevel, node.chunk);
		}
		else
		{
			ChunkPos rightChunkPos = node.chunk->GetPos() + ChunkPos(Chunk::WIDTH, 0);
			auto chunk = m_chunks.find(rightChunkPos);
			if (chunk != m_chunks.end())
			{
				TryAddLightNode(0, node.y, node.z, nextVoxelLightlevel, chunk->second);
			}
		}
		// Check front voxel
		if (node.z > 0)
		{
			TryAddLightNode(node.x, node.y, node.z - 1, nextVoxelLightlevel, node.chunk);
		}
		else
		{
			ChunkPos frontChunkPos = node.chunk->GetPos() + ChunkPos(0, -Chunk::WIDTH);
			auto chunk = m_chunks.find(frontChunkPos);
			if (chunk != m_chunks.end())
			{
				TryAddLightNode(node.x, node.y, Chunk::LAST_BLOCK_IDX, nextVoxelLightlevel, chunk->second);
			}
		}
		// Check back voxel
		if (node.z < Chunk::LAST_BLOCK_IDX)
		{
			TryAddLightNode(node.x, node.y, node.z + 1, nextVoxelLightlevel, node.chunk);
		}
		else
		{
			ChunkPos backChunkPos = node.chunk->GetPos() + ChunkPos(0, Chunk::WIDTH);
			auto chunk = m_chunks.find(backChunkPos);
			if (chunk != m_chunks.end())
			{
				TryAddLightNode(node.x, node.y, 0, nextVoxelLightlevel, chunk->second);
			}
		}
		// Check top voxel
		if (node.y < Chunk::HEIGHT) // TODO Check < Height
		{
			TryAddLightNode(node.x, node.y + 1, node.z, nextVoxelLightlevel, node.chunk);
		}
		// Check bottom voxel
		if (node.y > 0)
		{
			TryAddLightNode(node.x, node.y - 1, node.z, nextVoxelLightlevel, node.chunk);
		}
	}
}

inline void LightSolver::TryAddLightNode(int x, int y, int z, int lightLevel, std::shared_ptr<Chunk> chunk)
{
	int leftVoxelLightLevel = chunk->GetLightmapRef().GetChannel(x, y, z, m_channel);
	if (lightLevel <= leftVoxelLightLevel)
	{
		return;
	}
	if (m_blockManager.GetBlockById(chunk->GetBlock(x, y, z).GetId()).IsOpaque())
	{
		return;
	}
	chunk->GetLightmapRef().SetChannel(x, y, z, m_channel, lightLevel);
	m_addQueue.push({ x, y, z, chunk });
}