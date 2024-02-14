#include "pch.h"
#include "Lighting.h"
#include "ChunksManager.h"

Lighting::Lighting(ChunksManager* chunksManager, BlockManager& blockManager) :
	m_chunksManager(chunksManager),
	m_blockManager(blockManager)
{
	solverS = std::make_unique<LightSolver>(chunksManager, m_blockManager, 3);
	solverR = std::make_unique<LightSolver>(chunksManager, m_blockManager, 2);
	solverG = std::make_unique<LightSolver>(chunksManager, m_blockManager, 1);
	solverB = std::make_unique<LightSolver>(chunksManager, m_blockManager, 0);
}

void Lighting::HandleBlockSet(int x, int y, int z, std::shared_ptr<Chunk> chunk, ChunkBlock block)
{
	auto& blockType = m_blockManager.GetBlockById(block.GetId());
	auto& emission = blockType.GetEmission();
	solverR->Remove({ x, y, z, chunk->GetLightmapRef().GetR(x, y, z), chunk });
	solverG->Remove({ x, y, z, chunk->GetLightmapRef().GetG(x, y, z), chunk });
	solverB->Remove({ x, y, z, chunk->GetLightmapRef().GetB(x, y, z), chunk });
	chunk->GetLightmapRef().SetR(x, y, z, emission[0]);
	chunk->GetLightmapRef().SetG(x, y, z, emission[1]);
	chunk->GetLightmapRef().SetB(x, y, z, emission[2]);
	if (!blockType.IsOpaque())
	{
		// Если солнечный свет над блоком == 15, то пропускаем его ниже, т.к. блок прозрачный
		if (chunk->GetLightmapRef().GetS(x, y + 1, z) == 0xF)
		{
			for (int curY = y; curY > 0; curY--)
			{
				if (m_blockManager.GetBlockById(chunk->GetBlock(x, curY, z).GetId()).IsOpaque())
				{
					break;
				}
				chunk->GetLightmapRef().SetS(x, curY, z, 0xF);
				solverS->Add({ x, curY, z, chunk });
			}
		}
		AddToSolvers(x, y + 1, z, chunk);
		AddToSolvers(x, y - 1, z, chunk);
		if (x == Chunk::LAST_BLOCK_IDX)
		{
			auto rightChunk = m_chunksManager->GetChunkAt(chunk->GetX() + 1, chunk->GetZ());
			if (rightChunk)
			{
				AddToSolvers(0, y, z, rightChunk);
			}
		}
		else
		{
			AddToSolvers(x + 1, y, z, chunk);
		}
		if (x == 0)
		{
			auto leftChunk = m_chunksManager->GetChunkAt(chunk->GetX() - 1, chunk->GetZ());
			if (leftChunk)
			{
				AddToSolvers(Chunk::LAST_BLOCK_IDX, y, z, leftChunk);
			}
		}
		else
		{
			AddToSolvers(x - 1, y, z, chunk);
		}
		if (z == Chunk::LAST_BLOCK_IDX)
		{
			auto backChunk = m_chunksManager->GetChunkAt(chunk->GetX(), chunk->GetZ() + 1);
			if (backChunk)
			{
				AddToSolvers(x, y, 0, backChunk);
			}
		}
		else
		{
			AddToSolvers(x, y, z + 1, chunk);
		}
		if (z == 0)
		{
			auto frontChunk = m_chunksManager->GetChunkAt(chunk->GetX(), chunk->GetZ() - 1);
			if (frontChunk)
			{
				AddToSolvers(x, y, Chunk::LAST_BLOCK_IDX, frontChunk);
			}
		}
		else
		{
			AddToSolvers(x, y, z - 1, chunk);
		}
		Solve();
	}
	else
	{
		// Если же блок не прозрачный, то убираем весь солнечный свет ниже этого блока
		solverS->Remove({ x, y, z, chunk->GetLightmapRef().GetS(x, y, z), chunk });
		chunk->GetLightmapRef().SetS(x, y, z, 0);
		for (int curY = y - 1; y > 0; curY--)
		{
			if (m_blockManager.GetBlockById(chunk->GetBlock(x, curY, z).GetId()).IsOpaque())
			{
				break;
			}
			solverS->Remove({ x, curY, z, chunk->GetLightmapRef().GetS(x, curY, z), chunk });
			chunk->GetLightmapRef().SetS(x, curY, z, 0);
		}
		solverS->Solve();
		if (emission[0] || emission[1] || emission[2]) {
			solverR->Add({ x, y, z, chunk });
			solverG->Add({ x, y, z, chunk });
			solverB->Add({ x, y, z, chunk });
			solverR->Solve();
			solverG->Solve();
			solverB->Solve();
		}
	}
}

void Lighting::BuildSunlight(std::shared_ptr<Chunk> chunk)
{
	const int maxY = chunk->GetMaxY() + 1;
	for (int x = 0; x < Chunk::WIDTH; x++)
	{
		for (int z = 0; z < Chunk::WIDTH; z++)
		{
			int y = maxY;
			while (y > 0)
			{
				if (m_blockManager.GetBlockById(chunk->GetBlock(x, y, z).GetId()).IsOpaque())
				{
					break;
				}
				chunk->GetLightmapRef().SetS(x, y, z, 15);
				solverS->Add({x, y, z, chunk});
				y--;
			}
		}
	}
}

void Lighting::Solve()
{
	solverS->Solve();
	solverR->Solve();
	solverG->Solve();
	solverB->Solve();
}

inline void Lighting::AddToSolvers(int x, int y, int z, std::shared_ptr<Chunk> chunk)
{
	solverS->Add({ x, y, z, chunk });
	solverR->Add({ x, y, z, chunk }); 
	solverG->Add({ x, y, z, chunk }); 
	solverB->Add({ x, y, z, chunk }); 
}
