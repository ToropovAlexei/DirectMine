#pragma once

#include "WorldPos.hpp"
#include "ChunkBlock.h"

namespace WorldUtils
{
	inline WorldPos GetOffsetByBlockDirection(ChunkBlock::BlockDirection dir) noexcept
	{
		switch (dir)
		{
		case ChunkBlock::BlockDirection::East:
			return WorldPos(1, 0, 0);
		case ChunkBlock::BlockDirection::West:
			return WorldPos(-1, 0, 0);
		case ChunkBlock::BlockDirection::Up:
			return WorldPos(0, 1, 0);
		case ChunkBlock::BlockDirection::Down:
			return WorldPos(0, -1, 0);
		case ChunkBlock::BlockDirection::North:
			return WorldPos(0, 0, 1);
		case ChunkBlock::BlockDirection::South:
			return WorldPos(0, 0, -1);
		default:
			return WorldPos(0, 0, 0);
		}
	}

	inline ChunkBlock::BlockDirection GetNearBlockFaceToPoint(DirectX::XMFLOAT3& collisionPoint) noexcept
	{
		float blockX = std::floor(collisionPoint.x) + 0.5f;
		float blockY = std::floor(collisionPoint.y) + 0.5f;
		float blockZ = std::floor(collisionPoint.z) + 0.5f;
		float diffX = collisionPoint.x - blockX;
		float diffY = collisionPoint.y - blockY;
		float diffZ = collisionPoint.z - blockZ;

		float absDiffX = std::abs(diffX);
		float absDiffY = std::abs(diffY);
		float absDiffZ = std::abs(diffZ);

		float minDiff = std::max(absDiffX, std::max(absDiffY, absDiffZ));

		if (minDiff == absDiffX) {
			if (diffX < 0) {
				return ChunkBlock::BlockDirection::West;
			}
			return ChunkBlock::BlockDirection::East;
		}
		if (minDiff == absDiffY) {
			if (diffY < 0) {
				return ChunkBlock::BlockDirection::Down;
			}
			return ChunkBlock::BlockDirection::Up;
		}
		if (diffZ < 0) {
			return ChunkBlock::BlockDirection::South;
		}
		return ChunkBlock::BlockDirection::North;
	}
}


