#include "pch.h"
#include "LightSolver.h"
#include "Chunk.h"
#include "ChunksManager.h"

LightSolver::LightSolver(ChunksManager* chunksManager, BlockManager& blockManager, int channel):
	m_chunksManager(chunksManager),
	m_blockManager(blockManager),
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
			TryAddLightRemovalNode(node.x - 1, node.y, node.z, lightLevel, node.chunk);
		}
		else
		{
			auto chunk = m_chunksManager->GetChunkAt(node.chunk->GetX() - 1, node.chunk->GetZ());
			if (chunk)
			{
				TryAddLightRemovalNode(Chunk::LAST_BLOCK_IDX, node.y, node.z, lightLevel, chunk);
			}
		}
		// Check right voxel
		if (node.x < Chunk::LAST_BLOCK_IDX)
		{
			TryAddLightRemovalNode(node.x + 1, node.y, node.z, lightLevel, node.chunk);
		}
		else
		{
			auto chunk = m_chunksManager->GetChunkAt(node.chunk->GetX() + 1, node.chunk->GetZ());
			if (chunk)
			{
				TryAddLightRemovalNode(0, node.y, node.z, lightLevel, chunk);
			}
		}
		// Check front voxel
		if (node.z > 0)
		{
			TryAddLightRemovalNode(node.x, node.y, node.z - 1, lightLevel, node.chunk);
		}
		else
		{
			auto chunk = m_chunksManager->GetChunkAt(node.chunk->GetX(), node.chunk->GetZ() - 1);
			if (chunk)
			{
				TryAddLightRemovalNode(node.x, node.y, Chunk::LAST_BLOCK_IDX, lightLevel, chunk);
			}
		}
		// Check back voxel
		if (node.z < Chunk::LAST_BLOCK_IDX)
		{
			TryAddLightRemovalNode(node.x, node.y, node.z + 1, lightLevel, node.chunk);
		}
		else
		{
			auto chunk = m_chunksManager->GetChunkAt(node.chunk->GetX(), node.chunk->GetZ() + 1);
			if (chunk)
			{
				TryAddLightRemovalNode(node.x, node.y, 0, lightLevel, chunk);
			}
		}
		// Check top voxel
		if (node.y < Chunk::HEIGHT) // TODO Check < Height
		{
			TryAddLightRemovalNode(node.x, node.y + 1, node.z, lightLevel, node.chunk);
		}
		// Check bottom voxel
		if (node.y > 0)
		{
			TryAddLightRemovalNode(node.x, node.y - 1, node.z, lightLevel, node.chunk);
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
			auto chunk = m_chunksManager->GetChunkAt(node.chunk->GetX() - 1, node.chunk->GetZ());
			if (chunk)
			{
				TryAddLightNode(Chunk::LAST_BLOCK_IDX, node.y, node.z, nextVoxelLightlevel, chunk);
			}
		}
		// Check right voxel
		if (node.x < Chunk::LAST_BLOCK_IDX)
		{
			TryAddLightNode(node.x + 1, node.y, node.z, nextVoxelLightlevel, node.chunk);
		}
		else
		{
			auto chunk = m_chunksManager->GetChunkAt(node.chunk->GetX() + 1, node.chunk->GetZ());
			if (chunk)
			{
				TryAddLightNode(0, node.y, node.z, nextVoxelLightlevel, chunk);
			}
		}
		// Check front voxel
		if (node.z > 0)
		{
			TryAddLightNode(node.x, node.y, node.z - 1, nextVoxelLightlevel, node.chunk);
		}
		else
		{
			auto chunk = m_chunksManager->GetChunkAt(node.chunk->GetX(), node.chunk->GetZ() - 1);
			if (chunk)
			{
				TryAddLightNode(node.x, node.y, Chunk::LAST_BLOCK_IDX, nextVoxelLightlevel, chunk);
			}
		}
		// Check back voxel
		if (node.z < Chunk::LAST_BLOCK_IDX)
		{
			TryAddLightNode(node.x, node.y, node.z + 1, nextVoxelLightlevel, node.chunk);
		}
		else
		{
			auto chunk = m_chunksManager->GetChunkAt(node.chunk->GetX(), node.chunk->GetZ() + 1);
			if (chunk)
			{
				TryAddLightNode(node.x, node.y, 0, nextVoxelLightlevel, chunk);
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
	assert(x > -1);
	assert(y > -1);
	assert(z > -1);
	assert(x < Chunk::WIDTH);
	assert(z < Chunk::WIDTH);
	assert(y < Chunk::HEIGHT);
	int neighbourVoxelLightLevel = chunk->GetLightmapRef().GetChannel(x, y, z, m_channel);
	if (lightLevel <= neighbourVoxelLightLevel)
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

inline void LightSolver::TryAddLightRemovalNode(int x, int y, int z, int lightLevel, std::shared_ptr<Chunk> chunk)
{
	assert(x > -1);
	assert(y > -1);
	assert(z > -1);
	assert(x < Chunk::WIDTH);
	assert(z < Chunk::WIDTH);
	assert(y < Chunk::HEIGHT);
	int neighbourVoxelLightLevel = chunk->GetLightmapRef().GetChannel(x, y, z, m_channel);
	if (neighbourVoxelLightLevel != 0 && neighbourVoxelLightLevel < lightLevel)
	{
		chunk->GetLightmapRef().SetChannel(x, y, z, m_channel, 0);
		m_removeQueue.push({ x, y, z, neighbourVoxelLightLevel, chunk });
	}
	else if (neighbourVoxelLightLevel >= lightLevel)
	{
		m_addQueue.push({ x, y, z, chunk });
	}
}
