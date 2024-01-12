#pragma once

#include "pch.h"

struct WorldPos;

struct ChunkPos
{
	int x;
	int z;

	bool operator==(const ChunkPos& other) const
	{
		return x == other.x && z == other.z;
	}

	ChunkPos operator+(const ChunkPos& other) const
	{
		ChunkPos result;
		result.x = x + other.x;
		result.z = z + other.z;
		return result;
	}
	ChunkPos operator-(const ChunkPos& other) const
	{
		ChunkPos result;
		result.x = x - other.x;
		result.z = z - other.z;
		return result;
	}
};

struct ChunkPosHash
{
	std::size_t operator()(const ChunkPos& pos) const
	{
		std::size_t xHash = std::hash<int>{}(pos.x);
		std::size_t zHash = std::hash<int>{}(pos.z);

		std::size_t combinedHash = xHash ^ zHash;

		return combinedHash;
	}
};

struct WorldPos
{
	float x;
	float y;
	float z;

	bool operator==(const WorldPos& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	WorldPos operator+(const WorldPos& other) const
	{
		WorldPos result;
		result.x = x + other.x;
		result.y = y + other.y;
		result.z = z + other.z;
		return result;
	}
	WorldPos operator+(const ChunkPos& other) const
	{
		WorldPos result;
		result.x = x + other.x;
		result.y = y;
		result.z = z + other.z;
		return result;
	}
	WorldPos operator-(const WorldPos& other) const
	{
		WorldPos result;
		result.x = x - other.x;
		result.y = y - other.y;
		result.z = z - other.z;
		return result;
	}
};

struct WorldPosHash
{
	std::size_t operator()(const WorldPos& pos) const
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
