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
	LightRemovalNode(int x, int y, int z, int val, std::shared_ptr<Chunk> chunk) :
		x(x), y(y), z(z), val(val), chunk(chunk)
	{}

	int x;
	int y;
	int z;
	int val;
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
	inline void TryAddLightNode(int x, int y, int z, int lightLevel, std::shared_ptr<Chunk> chunk);

private:
	std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash>& m_chunks;
	BlockManager& m_blockManager;
	int m_channel;
	std::queue<LightNode> m_addQueue;
	std::queue<LightRemovalNode> m_removeQueue;
};

