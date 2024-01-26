#pragma once

#include "BlockManager.h"
#include "WorldPos.hpp"

class Chunk;

struct LightNode
{
	LightNode(int x, int y, int z, std::shared_ptr<Chunk> chunk)
		: x(x), y(y), z(z), chunk(chunk)
	{}

	int x;
	int y;
	int z;
	std::shared_ptr<Chunk> chunk;
};

struct LightRemovalNode {
	LightRemovalNode(int x, int y, int z, short v, Chunk* ch) : 
		x(x), y(y), z(z), val(v), chunk(ch) 
	{}

	int x;
	int y;
	int z;
	short val;
	std::shared_ptr<Chunk> chunk;
};

class LightSolver
{
public:
	LightSolver(std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& m_chunks, BlockManager& blockManager, int channel);

	void Add(LightNode node);
	void Remove(LightRemovalNode node);
	void Solve();

private:
	std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& m_chunks;
	BlockManager& m_blockManager;
	int m_channel;
	std::queue<LightNode> m_addQueue;
	std::queue<LightRemovalNode> m_removeQueue;
};

