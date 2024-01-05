#include "pch.h"
#include "DebugBlock.h"

DebugBlock::DebugBlock(DirectX::XMFLOAT3 pos) :
	Cube(pos)
{
}

CubeId DebugBlock::Id() const noexcept
{
	return CubeId::Debug;
}
