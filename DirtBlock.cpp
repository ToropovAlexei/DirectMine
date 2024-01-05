#include "pch.h"
#include "DirtBlock.h"

DirtBlock::DirtBlock(DirectX::XMFLOAT3 pos) :
	Cube(pos)
{
}

CubeId DirtBlock::Id() const noexcept
{
	return CubeId::Dirt;
}