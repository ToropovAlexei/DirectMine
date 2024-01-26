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
		//LightRemovalNode node = m_removeQueue.front();
		//node.chunk->SetIsModified(true);
		//m_removeQueue.pop();
		//int lightLevel = node.chunk->GetLightmapRef().GetChannel(node.x, node.y, node.z, m_channel);
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
			int leftVoxelLightLevel = node.chunk->GetLightmapRef().GetChannel(node.x - 1, node.y, node.z, m_channel);
			if ((nextVoxelLightlevel > leftVoxelLightLevel) && !(m_blockManager.GetBlockById(node.chunk->GetBlock(node.x - 1, node.y, node.z).GetId()).IsOpaque()))
			{
				node.chunk->GetLightmapRef().SetChannel(node.x - 1, node.y, node.z, m_channel, nextVoxelLightlevel);
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
				if ((nextVoxelLightlevel > leftVoxelLightLevel) &&
					!m_blockManager.GetBlockById(chunk->second->GetBlock(Chunk::WIDTH - 1, node.y, node.z).GetId()).IsOpaque())
				{
					chunk->second->GetLightmapRef().SetChannel(Chunk::WIDTH - 1, node.y, node.z, m_channel, nextVoxelLightlevel);
					m_addQueue.push({ Chunk::WIDTH - 1, node.y, node.z, chunk->second });
				}
			}
		}
		// Check right voxel
		if (node.x < Chunk::WIDTH - 1)
		{
			int rightVoxelLightLevel = node.chunk->GetLightmapRef().GetChannel(node.x + 1, node.y, node.z, m_channel);
			bool isOpaque = m_blockManager.GetBlockById(node.chunk->GetBlock(node.x + 1, node.y, node.z).GetId()).IsOpaque();
			if ((nextVoxelLightlevel > rightVoxelLightLevel) &&
				!isOpaque)
			{
				node.chunk->GetLightmapRef().SetChannel(node.x + 1, node.y, node.z, m_channel, nextVoxelLightlevel);
				m_addQueue.push({ node.x + 1, node.y, node.z, node.chunk });
			}
		}
		else
		{
			ChunkPos rightChunkPos = node.chunk->GetPos() + ChunkPos(Chunk::WIDTH, 0);
			auto chunk = m_chunks.find(rightChunkPos);
			if (chunk != m_chunks.end())
			{
				int rightVoxelLightLevel = chunk->second->GetLightmapRef().GetChannel(0, node.y, node.z, m_channel);
				if ((nextVoxelLightlevel > rightVoxelLightLevel) &&
					!m_blockManager.GetBlockById(chunk->second->GetBlock(0, node.y, node.z).GetId()).IsOpaque())
				{
					chunk->second->GetLightmapRef().SetChannel(0, node.y, node.z, m_channel, nextVoxelLightlevel);
					m_addQueue.push({ 0, node.y, node.z, chunk->second });
				}
			}
		}
		// Check front voxel
		if (node.z > 0)
		{
			int frontVoxelLightLevel = node.chunk->GetLightmapRef().GetChannel(node.x, node.y, node.z - 1, m_channel);
			if ((nextVoxelLightlevel > frontVoxelLightLevel) &&
				!m_blockManager.GetBlockById(node.chunk->GetBlock(node.x, node.y, node.z - 1).GetId()).IsOpaque())
			{
				node.chunk->GetLightmapRef().SetChannel(node.x, node.y, node.z - 1, m_channel, nextVoxelLightlevel);
				m_addQueue.push({ node.x, node.y, node.z - 1, node.chunk });
			}
		}
		else
		{
			ChunkPos frontChunkPos = node.chunk->GetPos() + ChunkPos(0, -Chunk::WIDTH);
			auto chunk = m_chunks.find(frontChunkPos);
			if (chunk != m_chunks.end())
			{
				int frontVoxelLightLevel = chunk->second->GetLightmapRef().GetChannel(node.x, node.y, Chunk::WIDTH - 1, m_channel);
				if ((nextVoxelLightlevel > frontVoxelLightLevel) &&
					!m_blockManager.GetBlockById(chunk->second->GetBlock(node.x, node.y, Chunk::WIDTH - 1).GetId()).IsOpaque())
				{
					chunk->second->GetLightmapRef().SetChannel(node.x, node.y, Chunk::WIDTH - 1, m_channel, nextVoxelLightlevel);
					m_addQueue.push({ node.x, node.y, Chunk::WIDTH - 1, chunk->second });
				}
			}
		}
		// Check back voxel
		if (node.z < Chunk::WIDTH - 1)
		{
			int backVoxelLightLevel = node.chunk->GetLightmapRef().GetChannel(node.x, node.y, node.z + 1, m_channel);
			if ((nextVoxelLightlevel > backVoxelLightLevel) &&
				!m_blockManager.GetBlockById(node.chunk->GetBlock(node.x, node.y, node.z + 1).GetId()).IsOpaque())
			{
				node.chunk->GetLightmapRef().SetChannel(node.x, node.y, node.z + 1, m_channel, nextVoxelLightlevel);
				m_addQueue.push({ node.x, node.y, node.z + 1, node.chunk });
			}
		}
		else
		{
			ChunkPos backChunkPos = node.chunk->GetPos() + ChunkPos(0, Chunk::WIDTH);
			auto chunk = m_chunks.find(backChunkPos);
			if (chunk != m_chunks.end())
			{
				int frontVoxelLightLevel = chunk->second->GetLightmapRef().GetChannel(node.x, node.y, 0, m_channel);
				if ((nextVoxelLightlevel > frontVoxelLightLevel) &&
					!m_blockManager.GetBlockById(chunk->second->GetBlock(node.x, node.y, 0).GetId()).IsOpaque())
				{
					chunk->second->GetLightmapRef().SetChannel(node.x, node.y, 0, m_channel, nextVoxelLightlevel);
					m_addQueue.push({ node.x, node.y, 0, chunk->second });
				}
			}
		}
		// Check top voxel
		if (node.y < Chunk::HEIGHT)
		{
			int topVoxelLightLevel = node.chunk->GetLightmapRef().GetChannel(node.x, node.y + 1, node.z, m_channel);
			if ((nextVoxelLightlevel > topVoxelLightLevel) &&
				!m_blockManager.GetBlockById(node.chunk->GetBlock(node.x, node.y + 1, node.z).GetId()).IsOpaque())
			{
				node.chunk->GetLightmapRef().SetChannel(node.x, node.y + 1, node.z, m_channel, nextVoxelLightlevel);
				m_addQueue.push({ node.x, node.y + 1, node.z, node.chunk });
			}
		}
		// Check bottom voxel
		if (node.y > 0)
		{
			int bottomVoxelLightLevel = node.chunk->GetLightmapRef().GetChannel(node.x, node.y - 1, node.z, m_channel);
			if ((nextVoxelLightlevel > bottomVoxelLightLevel) &&
				!m_blockManager.GetBlockById(node.chunk->GetBlock(node.x, node.y - 1, node.z).GetId()).IsOpaque())
			{
				node.chunk->GetLightmapRef().SetChannel(node.x, node.y - 1, node.z, m_channel, nextVoxelLightlevel);
				m_addQueue.push({ node.x, node.y - 1, node.z, node.chunk });
			}
		}
	}
}
