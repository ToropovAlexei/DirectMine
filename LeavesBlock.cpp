#include "pch.h"
#include "LeavesBlock.h"

LeavesBlock::LeavesBlock(DirectX::XMFLOAT3 pos) :
	Cube(pos)
{
}

CubeId LeavesBlock::Id() const noexcept
{
	return CubeId::Leaves;
}
