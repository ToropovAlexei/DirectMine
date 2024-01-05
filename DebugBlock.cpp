#include "pch.h"
#include "DebugBlock.h"

DebugBlock::DebugBlock(DirectX::XMMATRIX world) :
	Cube(world)
{
}

CubeId DebugBlock::Id() const noexcept
{
	return CubeId::Debug;
}
