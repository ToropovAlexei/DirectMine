#include "pch.h"
#include "DirtBlock.h"

DirtBlock::DirtBlock(DirectX::XMMATRIX world) :
	Cube(world)
{
}

CubeId DirtBlock::Id() const noexcept
{
	return CubeId::Dirt;
}