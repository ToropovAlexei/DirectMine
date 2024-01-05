#include "pch.h"
#include "LeavesBlock.h"

LeavesBlock::LeavesBlock(DirectX::XMMATRIX world) :
	Cube(world)
{
}

CubeId LeavesBlock::Id() const noexcept
{
	return CubeId::Leaves;
}
